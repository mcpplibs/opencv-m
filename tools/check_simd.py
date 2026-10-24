#!/usr/bin/env python3
"""Compare original-header and import-only SIMD consumers on the same target."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--mcpp", default="mcpp")
parser.add_argument("--toolchain")
parser.add_argument("--emulated", action="store_true", help="select the original SDK software implementation")
parser.add_argument("--profile", default="dev", choices=("dev", "release", "dist"))
args = parser.parse_args()
with tempfile.TemporaryDirectory(prefix="opencv-simd-oracle-") as directory:
    command = [args.mcpp, "test", "simd", "--features", "simd-emulated" if args.emulated else "simd", "--profile", args.profile,
               "--strict", "--timeout", "120"]
    if args.toolchain:
        command += ["--toolchain", args.toolchain]
    subprocess.run(command, cwd=Path(__file__).resolve().parents[1],
                   env=dict(os.environ, OPENCV_M_SIMD_OUTPUT=directory), check=True)
    sdk = (Path(directory) / "sdk.txt").read_bytes()
    module = (Path(directory) / "module.txt").read_bytes()
    assert sdk and sdk == module, "SIMD results differ from original SDK"
    print(f"Original SDK and module SIMD results match ({len(sdk)} bytes)")
    subprocess.run([os.sys.executable, str(Path(__file__).with_name("check_simd_surface.py"))], check=True)
