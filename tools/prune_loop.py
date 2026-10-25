#!/usr/bin/env python3
"""Report export compile failures without deleting or rewriting public APIs.

The historical filename is retained for existing tooling. This command now
builds once and writes diagnostics; every missing export requires explicit
review, an adapter or a justified exclusion. --log replays a captured build.
"""
import argparse
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
INC_ERR = re.compile(r"gen_exports/(\w+)\.inc:(\d+):(?:\d+:)?\s*(?:fatal )?error:\s*(.*)")
DECL = re.compile(r"^\s*\d+\s*\|\s*(?:export\s+)?using\s+([^;]+);")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("opencv_root", nargs="?", type=Path, help="legacy argument; must match the pinned SDK")
    parser.add_argument("--mcpp", default="mcpp")
    parser.add_argument("--features", default="all-modules")
    parser.add_argument("--toolchain")
    parser.add_argument("--log", type=Path, help="analyze an existing log instead of building")
    parser.add_argument("--report", type=Path, default=ROOT / "target/api-audit/export-diagnostics.json")
    args = parser.parse_args()
    if args.opencv_root and args.opencv_root.resolve() != ROOT / "upstream/opencv":
        parser.error("this diagnostic builds the checkout's pinned upstream/opencv only")
    if args.log:
        output = args.log.read_text()
        failed = bool(re.search(r"(?:fatal )?error:|build failed", output))
    else:
        command = [args.mcpp, "build", "--strict", "--no-color", "--features", args.features]
        if args.toolchain:
            command += ["--toolchain", args.toolchain]
        result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        output, failed = result.stdout, result.returncode != 0
    output = re.sub(r"\x1b\[[0-9;]*m", "", output)
    lines, errors = output.splitlines(), []
    for number, line in enumerate(lines):
        match = INC_ERR.search(line)
        if not match:
            continue
        # Use recorded context rather than today's possibly stale line number.
        declaration = next((found[1] for context in lines[number + 1:number + 4]
                            if (found := DECL.match(context))), None)
        errors.append({"module": match[1], "line": int(match[2]), "declaration": declaration,
                       "reason": match[3], "status": "requires-review"})
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps({"build": "failed" if failed else "passed", "export-errors": errors,
                                      "automatic-api-removals": 0}, indent=2) + "\n")
    args.report.with_suffix(".log").write_text(output)
    print(f"Build {'failed' if failed else 'passed'}; {len(errors)} export diagnostics; no API files changed.")
    print("Review report:", args.report)
    if failed and not errors:
        print("\n".join(lines[-30:]))
    raise SystemExit(1 if failed else 0)


if __name__ == "__main__":
    main()
