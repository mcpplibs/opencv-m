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
