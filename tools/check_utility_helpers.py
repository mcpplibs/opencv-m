#!/usr/bin/env python3
"""Compare independent header/module consumers of portable SDK helpers."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--mcpp", default="mcpp")
parser.add_argument("--toolchain")
args = parser.parse_args()
with tempfile.TemporaryDirectory(prefix="opencv-utility-oracle-") as directory:
    command = [args.mcpp, "test", "utility_helpers", "--features", "all-modules", "--strict", "--timeout", "120"]
    if args.toolchain:
        command += ["--toolchain", args.toolchain]
    env = dict(os.environ, OPENCV_M_UTILITY_OUTPUT=directory)
    subprocess.run(command, cwd=ROOT, env=env, check=True)
    sdk = (Path(directory) / "sdk.txt").read_bytes()
    module = (Path(directory) / "module.txt").read_bytes()
    assert sdk and sdk == module, "Portable helper results differ from the pinned SDK"
    print(f"SDK/member and module helper results match ({len(sdk)} bytes)")
