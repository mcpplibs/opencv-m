#!/usr/bin/env python3
"""Compare original SDK and imported Matx/Vec template-call contracts."""
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
args = parser.parse_args()
with tempfile.TemporaryDirectory(prefix="opencv-matx-template-oracle-") as directory:
    command = [args.mcpp, "test", "matx_templates", "--strict", "--timeout", "120"]
    if args.toolchain:
        command += ["--toolchain", args.toolchain]
    subprocess.run(command, cwd=ROOT,
                   env=dict(os.environ, OPENCV_M_MATX_OUTPUT=directory), check=True)
    sdk = (Path(directory) / "sdk.txt").read_bytes()
    module = (Path(directory) / "module.txt").read_bytes()
    assert sdk and sdk == module, "Matx/Vec template results differ from original SDK"
    print(f"Original SDK and module Matx/Vec results match ({len(sdk)} bytes)")
    commands = json.loads((ROOT / "compile_commands.json").read_text())
    consumer = next(entry for entry in commands if Path(entry["file"]).name == "matx_templates_test.cpp")
    output = ROOT / "target/api-audit/matx-templates"
    output.mkdir(parents=True, exist_ok=True)
    invalid = {
        "determinant-dimension": "cv::Matx33f m; (void)cv::determinant<float,2>(m);",
        "product-dimension": "cv::Matx<float,2,3> a; cv::Matx<float,3,2> b; (void)cv::operator*<float,2,3,3>(a,b);",
        "const-augmentation": "const cv::Matx22f a; cv::Matx22f b; (void)cv::operator+=<float,float,2,2>(a,b);",
    }
    for name, body in invalid.items():
        for label, preamble in (("sdk", "#include <opencv2/core.hpp>\n"),
                                ("module", "import mcpplibs.mwrap.opencv.core;\n")):
            probe = output / (name + "-" + label + ".cpp")
            probe.write_text(preamble + "void probe() { " + body + " }\n")
            result = subprocess.run([consumer["arguments"][0],
                                     *compiler_flags(consumer, keep_modules=label == "module"),
                                     "-x", "c++", "-fsyntax-only", str(probe)],
                                    cwd=consumer["directory"], capture_output=True, text=True)
            diagnostics = result.stdout + result.stderr
            probe.with_suffix(".log").write_text(diagnostics)
            assert result.returncode > 0 and "error:" in diagnostics, diagnostics
            assert not any(marker in diagnostics for marker in
                           ("frontend command failed", "internal compiler error", "PLEASE ATTACH", "Stack dump:")), diagnostics
    print("SDK/import contracts reject wrong determinant/product dimensions and const augmentation")
