"""Read the SIMD names enabled by the actual producer's preprocessor flags."""
import json
from pathlib import Path
import re
import subprocess


def enabled_names(root, commands, compiler_flags):
    producer = next(e for e in commands if Path(e["file"]).resolve() == root / "src/detail/simd.cppm")
    directory = root / "target/api-audit/simd"
    directory.mkdir(parents=True, exist_ok=True)
    expanded = subprocess.run([producer["arguments"][0], *compiler_flags(producer),
                               "-E", "-P", "-x", "c++", producer["file"]], cwd=producer["directory"],
                              capture_output=True, text=True, check=True).stdout
    (directory / "expanded.cpp").write_text(expanded)
    parts = re.split(r"export\s+(?:__preprocessed_)?module\s+mcpplibs\s*\.\s*mwrap\s*\.\s*opencv\s*\.\s*detail\s*\.\s*simd\s*;", expanded)
    assert len(parts) == 2, "SIMD preprocessor probe did not find the adapter module"
    names = set(re.findall(r"\busing\s+::(cv(?:::[A-Za-z_]\w*)+)\s*;", parts[1]))
    surface = json.loads((root / "port/api/simd-surface.json").read_text())
    names.update("cv::" + name for name in surface["legacy-adapters"])
    names.update("cv::" + name for name in surface["broadcast-adapters"])
    if "clang" not in Path(producer["arguments"][0]).name:
        names.update("cv::" + name for name in surface["gcc-memory-bridges"])
    assert len(names) >= 215, f"Unexpectedly small SIMD surface: {len(names)}"
    return names
