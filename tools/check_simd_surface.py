#!/usr/bin/env python3
"""Resolve every enabled SIMD export through direct and aggregate imports."""
import json
from pathlib import Path
import subprocess

from audit_api import compiler_flags
from simd_surface import enabled_names

ROOT = Path(__file__).resolve().parents[1]
commands = json.loads((ROOT / "compile_commands.json").read_text())
consumer = next(e for e in commands if Path(e["file"]).name == "simd_test.cpp")
directory = ROOT / "target/api-audit/simd"
directory.mkdir(parents=True, exist_ok=True)
names = enabled_names(ROOT, commands, compiler_flags)
for label, module in (("direct", "mcpplibs.mwrap.opencv.core.simd"), ("aggregate", "mcpplibs.mwrap.opencv")):
    probe = directory / (label + ".cpp")
    probe.write_text("import " + module + ";\n" + "\n".join(
        f"namespace sdk_export_{i} {{ using ::{name}; }}" for i, name in enumerate(sorted(names))) + "\n")
    result = subprocess.run([consumer["arguments"][0], *compiler_flags(consumer, keep_modules=True),
                             "-x", "c++", "-fsyntax-only", str(probe)], cwd=consumer["directory"],
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    (directory / (label + ".log")).write_text(result.stdout)
    if result.returncode:
        raise SystemExit(result.stdout)
print(f"Resolved {len(names)} SIMD names through direct and aggregate imports")
for label, preamble in (
    ("sdk", "#if defined(MCPP_FEATURE_SIMD_EMULATED)\n#define CV_FORCE_SIMD128_CPP 1\n#endif\n#include <opencv2/core/simd_intrinsics.hpp>\n"),
    ("module", "import mcpplibs.mwrap.opencv.core.simd;\n"),
):
    for function in ("v_setall", "vx_setall"):
        probe = directory / (label + "-unsupported-" + function + ".cpp")
        probe.write_text(preamble + "void probe() { (void)cv::" + function + "('x'); }\n")
        result = subprocess.run([consumer["arguments"][0], *compiler_flags(consumer, keep_modules=label == "module"),
                                 "-x", "c++", "-fsyntax-only", str(probe)], cwd=consumer["directory"],
                                capture_output=True, text=True)
        diagnostics = result.stdout + result.stderr
        probe.with_suffix(".log").write_text(diagnostics)
        assert result.returncode > 0 and "error:" in diagnostics, diagnostics
        assert not any(marker in diagnostics for marker in
                       ("frontend command failed", "internal compiler error", "Stack dump:")), diagnostics
print("SDK/import broadcasts reject unsupported plain-char traits")
if "cv::v_reg" in names:
    # The pinned SDK itself has no V_TypeTraits<T>::reinterpret_uint. Preserve
    # that template's visibility, and verify the same instantiation diagnostic
    # independently on original headers and imports instead of deleting it.
    for label, preamble in (
        ("sdk", "#define CV_FORCE_SIMD128_CPP 1\n#include <opencv2/core/simd_intrinsics.hpp>\n"),
        ("module", "import mcpplibs.mwrap.opencv.core.simd;\n"),
    ):
        probe = directory / (label + "-reinterpret-uint.cpp")
        probe.write_text(preamble + "void probe() { cv::v_reg<float, 8> value; (void)cv::v_reinterpret_as_uint(value); }\n")
        result = subprocess.run([consumer["arguments"][0], *compiler_flags(consumer, keep_modules=True),
                                 "-x", "c++", "-fsyntax-only", str(probe)], cwd=consumer["directory"],
                                capture_output=True, text=True)
        diagnostics = result.stdout + result.stderr
        (directory / (label + "-reinterpret-uint.log")).write_text(diagnostics)
        assert result.returncode and "reinterpret_uint" in diagnostics, diagnostics
    print("Original SDK and module preserve the pinned reinterpret_uint instantiation failure")
