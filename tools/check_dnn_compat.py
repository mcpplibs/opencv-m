#!/usr/bin/env python3
"""Compare original DNN Arg equality and unavailable legacy backend behavior."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--mcpp", default="mcpp")
parser.add_argument("--toolchain")
args = parser.parse_args()
with tempfile.TemporaryDirectory(prefix="opencv-dnn-compat-oracle-") as directory:
    command = [args.mcpp, "test", "dnn_compat", "--features", "all-modules,simd-emulated",
               "--strict", "--timeout", "120"]
    if args.toolchain:
        command += ["--toolchain", args.toolchain]
    subprocess.run(command, cwd=Path(__file__).resolve().parents[1],
                   env=dict(os.environ, OPENCV_M_DNN_COMPAT_OUTPUT=directory), check=True)
    sdk = (Path(directory) / "sdk.txt").read_bytes()
    module = (Path(directory) / "module.txt").read_bytes()
    assert sdk and sdk == module, "DNN compatibility behavior differs from original SDK"
    print(f"SDK and module DNN equality/backend contracts match ({len(sdk)} bytes)")
