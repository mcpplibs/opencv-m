#!/usr/bin/env python3
"""Report original-name visibility for inventory entities outside using lists.

This is a diagnostic, not a completeness gate. A visible name does not prove
overload/signature or behavior parity; rejected names still need source review.
"""
import argparse
import json
from pathlib import Path
import re
import subprocess

from audit_api import compiler_flags

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--commands", type=Path, default=Path("compile_commands.json"))
    parser.add_argument("--inventory", type=Path, default=ROOT / "target/api-audit/inventory.json")
    parser.add_argument("--output", type=Path, default=ROOT / "target/api-audit/unlisted-import-probe/report.json")
    args = parser.parse_args()
    inventory = json.loads(args.inventory.read_text())
    commands = json.loads(args.commands.read_text())
    consumer = next(item for item in commands if item["file"].endswith("/tests/main_modules_test.cpp"))
    assert "clang" in Path(consumer["arguments"][0]).name, "Use a recorded Clang all-module consumer"
    flags = compiler_flags(consumer, keep_modules=True)
    features = sorted({flag.removeprefix("-DMCPP_FEATURE_").lower().replace("_", "-")
                       for flag in flags if flag.startswith("-DMCPP_FEATURE_")})
    assert features == inventory["profile"]["features"], "Inventory and consumer feature profiles differ"
    # Explicit legacy import is opt-in. Retain only BMI mappings from this same
    # consumer profile, never stale commands from another feature/toolchain run.
    imports = ["mcpplibs.mwrap.opencv"]
    if "core.simd.legacy" in inventory.get("optional-facades", []):
        imports.append("mcpplibs.mwrap.opencv.core.simd.legacy")
        for entry in commands:
            if entry["directory"] == consumer["directory"]:
                for flag in entry["arguments"]:
                    if flag.startswith("-fmodule-file=") and flag not in flags:
                        flags.append(flag)
    work = args.output.parent
    work.mkdir(parents=True, exist_ok=True)
    source = (work / "probe.cpp").resolve()
    names = sorted(name for name, item in inventory["entities"].items() if not item["listed-export"])
    remaining, failures = names[:], {}
    for attempt in range(4):
        source.write_text("".join(f"import {name};\n" for name in imports) + "".join(
            f"namespace unlisted_{number} {{ using ::{name}; }}\n" for number, name in enumerate(remaining)))
        command = [consumer["arguments"][0], *flags, "-ferror-limit=0", "-x", "c++", "-fsyntax-only", str(source)]
        result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (work / f"attempt-{attempt}.log").write_text(result.stdout)
        if not result.returncode:
            break
        rejected = {}
        for line in result.stdout.splitlines():
            match = re.match(re.escape(str(source)) + r":(\d+):\d+: (?:fatal )?error: (.*)", line)
            if match:
                number = int(match[1]) - len(imports) - 1
                assert 0 <= number < len(remaining), line
                rejected.setdefault(remaining[number], []).append(match[2])
        assert rejected, result.stdout
        failures.update(rejected)
        remaining = [name for name in remaining if name not in rejected]
    else:
        raise RuntimeError("Name probe did not converge; inspect retained compiler diagnostics")
    assert not result.returncode
    assert set(remaining) | set(failures) == set(names)
    report = {"scope": "Original qualified-name lookup only; not overload, signature or behavior completeness",
              "profile": inventory["profile"], "imports": imports, "commands": str(args.commands.resolve()),
              "total": len(names), "visible": remaining, "not-visible": failures}
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    print(f"Unlisted original names: {len(names)}; visible: {len(remaining)}; not visible: {len(failures)}")
    print("Rejected names require source classification; successful names still require behavioral coverage")


if __name__ == "__main__":
    main()
