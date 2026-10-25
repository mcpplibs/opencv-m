#!/usr/bin/env python3
"""Inventory facade-visible upstream C++ declarations with the resolved Clang.

This is an audit, not the Python binding parser. It includes namespace-level
classes/templates, overloads, aliases, unscoped enumerators and global helpers.
Header coverage is reported separately: an existing facade cannot prove that
headers it does not include are covered. Raw ASTs stay under target/.
"""
import argparse
import json
from pathlib import Path
import re
import subprocess


KINDS = {"CXXRecordDecl", "RecordDecl", "ClassTemplateDecl", "FunctionTemplateDecl",
         "FunctionDecl", "TypedefDecl", "TypeAliasDecl", "TypeAliasTemplateDecl",
         "EnumDecl", "VarDecl", "UsingDecl"}


def namespace_const(node):
    """Recognize namespace-level const objects with implicit internal linkage.

    Clang omits storageClass on these. Pointee constness does not make a
    pointer/reference object const; aliases use their desugared type.
    """
    if node.get("inline") or node.get("storageClass") == "extern":
        return False
    type_ = node.get("type", {})
    spelling = type_.get("desugaredQualType", type_.get("qualType", ""))
    spelling = re.sub(r"(?:\[[^]]*\])+$", "", spelling).strip()
    if "&" in spelling:
        return False
    if "*" in spelling:
        pointer = re.search(r"\([^)]*\*([^)]*)\)", spelling)
        tail = pointer[1] if pointer else spelling.rsplit("*", 1)[1]
        return "const" in tail.split()
    return spelling.startswith("const ") or spelling.endswith(" const")


def compiler_flags(entry, keep_modules=False):
    arguments = entry["arguments"]
    flags = []
    number = 1
    while number < len(arguments):
        flag = arguments[number]
        if flag in {"-o", "-MF", "-MT", "-MQ", "-x"}:
            number += 2
            continue
        if (flag in {"-c", "-MMD", "-MD"} or flag == entry["file"] or flag.startswith("-fmodule-output")
                or (not keep_modules and flag.startswith(
                    ("-fmodule-file=", "-fprebuilt-module-path=", "-fmodule-mapper=")))):
            number += 1
            continue
        flags.append(flag)
        number += 1
    return flags


def optional_simd_facades(root, commands, active_arguments):
    # CDBs can retain commands for previously enabled optional source files.
    # The active root's feature flags, not file presence alone, select this API.
    if "-DMCPP_FEATURE_SIMD" not in active_arguments:
        return []
    result = []
    compiled = {Path(item["file"]).resolve() for item in commands}
    for source, module in (("simd", "core.simd"), ("simd_legacy", "core.simd.legacy")):
        path = root / "src/detail" / (source + ".cppm")
        assert path in compiled, f"Active SIMD facade has no compile command: {path}"
        result.append((module, path))
    return result


def ast_declarations(lines, capture=None):
    """Read one top-level Clang declaration at a time, not the huge TU tree.

    Clang's JSON printer indents translation-unit children four spaces.
    Class/function bodies remain inside their containing declaration, so peak
    memory is bounded by one declaration rather than the entire standard library.
    """
    block = None
    for line in lines:
        if capture:
            capture.write(line)
        if block is None:
            if line.startswith("    {"):
                block = [line]
            continue
        block.append(line)
        if line.rstrip() in ("    }", "    },"):
            text = "".join(block).rstrip().removesuffix(",")
            block = None
            yield json.loads(text)
    assert block is None, "Truncated AST declaration"


def inventory(declarations, root):
    entities = {}
    last_file = ""
    namespaces = {}
    imports = []
    variable_linkage = {}

    def restore_locations(value):
        """Clang elides repeated file names across the entire JSON stream.

        Traverse every location, including class/function bodies and ranges,
        before filtering declarations. includedFrom is an include-stack hint,
        not the physical source location and must not update the cursor.
        """
        nonlocal last_file
        if isinstance(value, list):
            for child in value:
                restore_locations(child)
        elif isinstance(value, dict):
            if "offset" in value:
                if value.get("file"):
                    last_file = value["file"]
                elif last_file:
                    value["file"] = last_file
            for key, child in value.items():
                if key != "includedFrom":
                    restore_locations(child)

    def source_file(node, parent_file):
        loc = node.get("loc", {})
        # Macro-created declaration names belong to their expansion header.
        loc = loc.get("expansionLoc", loc)
        file = loc.get("file") or parent_file
        return file, loc.get("line")

    def visit(nodes, namespace="", parent_file="", namespace_ids=None):
        namespace_ids = namespace_ids or set()
        for node in nodes:
            kind = node.get("kind")
            file, line = source_file(node, parent_file)
            if kind == "NamespaceDecl":
                name = node.get("name", "")
                if not name:
                    continue  # anonymous namespaces cannot be imported by name
                path = namespace if node.get("isInline") else "::".join(filter(None, (namespace, name)))
                namespaces[node.get("id")] = path
                # Source ownership below filters standard-library internals,
                # while preserving cvflann and OpenCV-owned std extensions.
                visit(node.get("inner", []), path, file, {node.get("id")})
                continue
            if kind == "UsingDirectiveDecl":
                target = node.get("nominatedNamespace", {})
                if namespace.startswith("cv"):
                    imports.append((namespace, target.get("id"), target.get("name", "")))
                continue
            if kind == "LinkageSpecDecl":
                visit(node.get("inner", []), namespace, file, namespace_ids)
                continue
            if kind not in KINDS or node.get("isImplicit"):
                continue
            if node.get("parentDeclContextId") and node["parentDeclContextId"] not in namespace_ids:
                continue  # an out-of-line class member rides with its class
            if not file:
                continue
            variable_internal = False
            if kind == "VarDecl":
                previous = node.get("previousDecl")
                variable_internal = variable_linkage.get(previous,
                    node.get("storageClass") == "static" or namespace_const(node))
                if node.get("id"):
                    variable_linkage[node["id"]] = variable_internal
            try:
                relative = Path(file).resolve().relative_to(root / "upstream/opencv")
            except ValueError:
                continue
            match = re.match(r"modules/([^/]+)/include/", relative.as_posix())
            if not match or "/private" in relative.as_posix() or ".private." in relative.name:
                continue
            name = node.get("name", "")
            if kind == "UsingDecl":
                name = name.rsplit("::", 1)[-1]

            def add(leaf, decl):
                if not leaf or leaf.startswith("__") or (not leaf.isidentifier() and not leaf.startswith("operator")):
                    return
                qualified = "::".join(filter(None, (namespace, leaf)))
                item = entities.setdefault(qualified, {"kinds": set(), "headers": set(), "signatures": set(),
                                                       "modules": set(), "internal-linkage": False})
                item["kinds"].add(decl["kind"])
                item["headers"].add(relative.as_posix())
                item["modules"].add(match[1])
                if decl.get("type", {}).get("qualType"):
                    item["signatures"].add(decl["type"]["qualType"])
                if decl["kind"] in {"FunctionTemplateDecl", "TypeAliasTemplateDecl"}:
                    for child in decl.get("inner", []):
                        if child.get("kind") in {"FunctionDecl", "TypeAliasDecl"} and child.get("type", {}).get("qualType"):
                            item["signatures"].add(child["type"]["qualType"])
                item["internal-linkage"] |= variable_internal or decl.get("storageClass") == "static" or any(
                    child.get("storageClass") == "static" for child in decl.get("inner", [])
                    if child.get("kind") == "FunctionDecl")

            add(name, node)
            if kind == "EnumDecl" and not node.get("scopedEnumTag"):
                for child in node.get("inner", []):
                    if child.get("kind") == "EnumConstantDecl":
                        add(child.get("name", ""), child)
    for node in declarations:
        restore_locations(node)
        visit([node])
        del node
    aliases = []
    for public, identifier, name in imports:
        canonical = namespaces.get(identifier, public + "::" + name)
        if canonical != public and canonical.startswith(public + "::"):
            aliases.append((canonical, public))
    for name, item in entities.items():
        public_names, pending = {name}, [name]
        while pending:
            candidate = pending.pop()
            for canonical, public in aliases:
                if candidate.startswith(canonical + "::"):
                    alias = public + candidate[len(canonical):]
                    if alias not in public_names:
                        public_names.add(alias)
                        pending.append(alias)
        item["public-name"] = min(public_names, key=lambda value: (value.count("::"), len(value)))
    return {name: {key: sorted(value) if isinstance(value, set) else value for key, value in item.items()}
            for name, item in sorted(entities.items())}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--commands", type=Path, default=Path("compile_commands.json"))
    parser.add_argument("--output", type=Path, default=Path("target/api-audit/inventory.json"))
    parser.add_argument("--reuse-ast", action="store_true", help="re-analyze the same captured AST during audit development")
    parser.add_argument("--keep-ast", action="store_true", help="retain the large raw compiler AST; normally it is streamed")
    parser.add_argument("--verify-exports", action="store_true",
                        help="compile an import-only consumer for every listed export using resolved test BMIs")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    commands = json.loads(args.commands.read_text())
    entry = next(item for item in commands if item["file"].endswith("/src/core.cppm"))
    assert "clang" in Path(entry["arguments"][0]).name, "Capture an all-modules Clang build first"
    for module in (root / "port/config/modules.txt").read_text().split():
        if module != "core":
            assert "-DHAVE_OPENCV_" + module.upper() in entry["arguments"], "Use --features all-modules"
    work = root / "target/api-audit"
    work.mkdir(parents=True, exist_ok=True)
    probe = work / "upstream.cpp"
    prefixes = []
    for module in (root / "port/config/modules.txt").read_text().split():
        facade = root / "src/detail" / (module + ".cppm")
        if not facade.exists():
            facade = root / "src" / (module + ".cppm")
        source = facade.read_text()
        prefixes.append(source.split("export module", 1)[0].replace("module;", ""))
    optional_facades = []
    for module, facade in optional_simd_facades(root, commands, entry["arguments"]):
        source = facade.read_text()
        prefixes.append(source.split("export module", 1)[0].replace("module;", ""))
        optional_facades.append(module)
    probe.write_text("\n".join(prefixes))
    ast_path, includes_path = work / "upstream.ast.json", work / "includes.log"
    if not args.reuse_ast:
        command = [entry["arguments"][0], *compiler_flags(entry), "-x", "c++", "-H",
                   "-fsyntax-only", "-Xclang", "-ast-dump=json", str(probe)]
        print("Auditing the exact resolved Clang/header configuration", flush=True)
        with includes_path.open("w") as err:
            with subprocess.Popen(command, cwd=root, stdout=subprocess.PIPE, stderr=err, text=True) as process:
                if args.keep_ast:
                    with ast_path.open("w") as capture:
                        entities = inventory(ast_declarations(process.stdout, capture), root)
                else:
                    entities = inventory(ast_declarations(process.stdout), root)
                code = process.wait()
        if code:
            raise SystemExit(includes_path.read_text()[-12000:])
    else:
        with ast_path.open() as source:
            entities = inventory(ast_declarations(source), root)
    using_names = set()
    for path in list((root / "src/gen_exports").glob("*.inc")) + list((root / "src").glob("*.cppm")):
        using_names.update(re.findall(r"^\s*(?:export\s+)?using\s+(?:::)?([A-Za-z_][^;\s]*)\s*;", path.read_text(), re.M))
    if "core.simd" in optional_facades:
        from simd_surface import enabled_names
        using_names.update(enabled_names(root, commands, compiler_flags))
    for name, item in entities.items():
        item["listed-export"] = name in using_names or item["public-name"] in using_names
    loaded = set()
    for line in includes_path.read_text().splitlines():
        match = re.match(r"\.+ (.+?)(?: \(.*\))?$", line)
        if match:
            try:
                loaded.add(Path(match[1]).resolve().relative_to(root / "upstream/opencv").as_posix())
            except ValueError:
                pass
    headers = {}
    for path in sorted((root / "upstream/opencv/modules").glob("*/include/opencv2/**/*")):
        if path.suffix not in {".hpp", ".h"} or not path.is_file():
            continue
        relative = path.relative_to(root / "upstream/opencv").as_posix()
        if "/private" in relative or ".private." in relative:
            headers[relative] = "private-implementation"
        else:
            headers[relative] = "loaded" if relative in loaded else "not-in-facade"
    profile = {"target": Path(entry["directory"]).parent.name,
               "compiler": Path(entry["arguments"][0]).name,
               "features": sorted({flag.removeprefix("-DMCPP_FEATURE_").lower().replace("_", "-")
                                   for flag in entry["arguments"] if flag.startswith("-DMCPP_FEATURE_")})}
    report = {"opencv": "5.0.0", "profile": profile,
              "scope": "Declarations visible from current facades; not-in-facade headers still require review",
              "entities": entities, "headers": headers}
    report["optional-facades"] = optional_facades
    if args.verify_exports:
        consumer = next((item for item in commands if item["file"].endswith("/tests/main_modules_test.cpp")), None)
        assert consumer is not None, "Run an all-modules Clang test first to capture the consumer compile command"
        probe = work / "exports.cpp"
        probe.write_text("import mcpplibs.mwrap.opencv;\n" + "\n".join(
            f"namespace export_probe_{number} {{ using ::{name}; }}"
            for number, name in enumerate(sorted(using_names))) + "\n")
        command = [consumer["arguments"][0], *compiler_flags(consumer, keep_modules=True),
                   "-x", "c++", "-fsyntax-only", str(probe)]
        result = subprocess.run(command, cwd=root, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        (work / "export-consumer.log").write_text(result.stdout)
        if result.returncode:
            raise SystemExit(result.stdout)
        verified_modules = []
        for module in (root / "port/config/modules.txt").read_text().split():
            module_names = set(re.findall(r"^\s*(?:export\s+)?using\s+(?:::)?([A-Za-z_][^;\s]*)\s*;",
                                         (root / "src/gen_exports" / (module + ".inc")).read_text(), re.M))
            probe.write_text("import mcpplibs.mwrap.opencv." + module + ";\n" +
                             "void core_visibility() { cv::Mat image; cv::Point point; cv::Size size; }\n" +
                             "\n".join(f"namespace export_probe_{number} {{ using ::{name}; }}"
                                       for number, name in enumerate(sorted(module_names))) + "\n")
            result = subprocess.run(command, cwd=root, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
            with (work / "export-consumer.log").open("a") as log:
                log.write(f"\nDirect import {module}: {len(module_names)} own export names\n" + result.stdout)
            if result.returncode:
                raise SystemExit(f"Direct import failed: {module}\n" + result.stdout)
            verified_modules.append(module)
        report["export-consumer"] = {"verified-names": len(using_names),
                                     "verified-direct-modules": verified_modules,
                                     "scope": "Root/direct-module name lookup and core dependency visibility; not overload instantiation or behavior coverage"}
        print(f"Import-only consumers resolved all {len(using_names)} listed root exports and {len(verified_modules)} direct module surfaces")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    unlisted = sum(not item["listed-export"] for item in entities.values())
    print(f"Inventoried {len(entities)} namespace/global entities; {unlisted} are not in export-using lists")
    print("Manual adapters must be verified by a module consumer; an absent using-list entry alone is not proof of a missing API")
    print(f"Header candidates: {len(headers)}; {sum(value == 'not-in-facade' for value in headers.values())} not in facades; "
          f"{sum(value == 'private-implementation' for value in headers.values())} private implementation headers")


if __name__ == "__main__":
    main()
