#!/usr/bin/env python3
"""Reject private x86 ISA flags on public/internal wrapper module BMIs."""
import json
from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
commands = json.loads((root / "compile_commands.json").read_text())
checked = 0
failures = []
for entry in commands:
    path = Path(entry["file"]).resolve()
    if path.suffix != ".cppm" or not path.is_relative_to(root / "src"):
        continue
    checked += 1
    flags = [flag for flag in entry["arguments"] if re.match(r"-m(?:sse|avx|arch=)", flag)]
    if flags:
        failures.append(f"{path.relative_to(root)}: {', '.join(flags)}")
if not checked:
    raise SystemExit("No wrapper module compile commands found; build first")
if failures:
    raise SystemExit("Non-baseline wrapper module flags:\n" + "\n".join(failures))
print(f"Verified baseline ISA flags for {checked} wrapper module interfaces")
