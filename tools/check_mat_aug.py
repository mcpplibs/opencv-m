#!/usr/bin/env python3
"""Compare 49 mutable/const SDK Mat assignment families with module imports."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--mcpp", default="mcpp")
parser.add_argument("--toolchain")
args = parser.parse_args()
with tempfile.TemporaryDirectory(prefix="opencv-mat-aug-oracle-") as directory:
    command = [args.mcpp, "test", "mat_aug", "--features", "all-modules,simd-emulated",
               "--strict", "--timeout", "120"]
    if args.toolchain:
        command += ["--toolchain", args.toolchain]
    subprocess.run(command, cwd=Path(__file__).resolve().parents[1],
                   env=dict(os.environ, OPENCV_M_MAT_AUG_OUTPUT=directory), check=True)
    sdk = (Path(directory) / "sdk.txt").read_bytes()
    module = (Path(directory) / "module.txt").read_bytes()
    assert sdk and sdk == module, "Mat assignment behavior differs from original SDK"
    assert len(sdk.splitlines()) == 99, "49 mutable/const cases and signature checks required"
    print(f"SDK and module Mat assignments match ({len(sdk)} bytes; 98 operand/const cases)")
