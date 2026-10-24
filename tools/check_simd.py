#!/usr/bin/env python3
"""Compare original-header and import-only SIMD consumers on the same target."""
import argparse
import json
import os
from pathlib import Path
import subprocess
import tempfile

from audit_api import compiler_flags

ROOT = Path(__file__).resolve().parents[1]

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
    legacy_sdk = (Path(directory) / "sdk-legacy.txt").read_bytes()
    legacy_module = (Path(directory) / "module-legacy.txt").read_bytes()
    assert legacy_sdk and legacy_sdk == legacy_module, "Legacy SIMD operators differ from original SDK"
    print(f"All 21 original legacy operators match SDK behavior ({len(legacy_sdk)} bytes)")
    subprocess.run([os.sys.executable, str(Path(__file__).with_name("check_simd_surface.py"))], check=True)
    commands = json.loads((ROOT / "compile_commands.json").read_text())
    consumer = next(entry for entry in commands if Path(entry["file"]).name == "simd_test.cpp")
    for module_name in ("mcpplibs.mwrap.opencv.core.simd", "mcpplibs.mwrap.opencv"):
        probe = Path(directory) / "legacy-opt-in.cpp"
        probe.write_text("import " + module_name + ";\nusing cv::hal::operator+;\n")
        result = subprocess.run([consumer["arguments"][0], *compiler_flags(consumer, keep_modules=True),
                                 "-x", "c++", "-fsyntax-only", str(probe)],
                                cwd=consumer["directory"], capture_output=True, text=True)
        diagnostics = result.stdout + result.stderr
        assert result.returncode > 0 and "operator+" in diagnostics, diagnostics
        assert not any(value in diagnostics for value in ("frontend command failed", "internal compiler error", "Stack dump:")), diagnostics
    print("Base SIMD and root imports keep legacy operators opt-in")
