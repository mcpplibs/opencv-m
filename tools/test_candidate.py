#!/usr/bin/env python3
"""Test the checkout against an unpublished SDK/wrapper index in isolation."""
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import tomllib


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--index", required=True, type=Path)
    parser.add_argument("--mcpp", default="mcpp")
    parser.add_argument("--toolchain", default="llvm@23.1.3")
    parser.add_argument("--features", default="dnn-onnxruntime")
    parser.add_argument("--filter", help="run matching test binaries; omit to run the complete suite")
    parser.add_argument("--target", help="explicit target triple for native or cross verification")
    parser.add_argument("--absent-dependency", action="append", default=[],
                        help="require an optional package to be absent from the lock and compilation database")
    parser.add_argument("--keep", action="store_true", help="retain the exact staged project for diagnosis")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    index = args.index.resolve()
    subprocess.run([os.sys.executable, str(index / "tests/prepare_mwrap_indices.py")], cwd=index, check=True)
    class Workspace:
        def __enter__(self):
            self.directory = tempfile.mkdtemp(prefix="mwrap-opencv-candidate-")
            return self.directory
        def __exit__(self, kind, value, traceback):
            if args.keep:
                print("Retained candidate:", self.directory, flush=True)
            else:
                shutil.rmtree(self.directory)
    with Workspace() as directory:
        source = Path(directory) / "source"
        shutil.copytree(root, source, ignore=shutil.ignore_patterns(
            ".git", "target", ".mcpp", ".xlings", "__pycache__", "mcpp.lock", "compile_commands.json",
            ".candidate-index", "mcpp-2026.*"))
        with (source / "mcpp.toml").open("a") as manifest:
            manifest.write('\n[indices]\n')
            for namespace in ("compat", "mcpplibs.mwrap"):
                path = index / "tests/mwrap/.indices" / namespace
                manifest.write('"' + namespace + '" = { path = "' + path.as_posix() + '" }\n')
        command = [args.mcpp, "test"]
        if args.filter:
            command.append(args.filter)
        command += ["--strict", "--features", args.features,
                    "--toolchain", args.toolchain, "--timeout", "120"]
        if args.target:
            command += ["--target", args.target]
        print("+", " ".join(command), flush=True)
        subprocess.run(command, cwd=source, check=True)
        if args.absent_dependency:
            # mcpp emits no lock for an empty production graph; test-only
            # dependencies are excluded from it. Check actual units as well.
            lock = source / "mcpp.lock"
            packages = tomllib.loads(lock.read_text()).get("package", {}) if lock.exists() else {}
            commands = json.loads((source / "compile_commands.json").read_text())
            for identity in args.absent_dependency:
                assert identity not in packages, f"Unexpected optional dependency: {identity}"
                namespace, name = identity.rsplit(".", 1)
                marker = f"/{namespace}-x-{name}/"
                assert not any(marker in entry["file"].replace("\\", "/") for entry in commands), identity
                print("Verified absent optional dependency:", identity, flush=True)


if __name__ == "__main__":
    main()
