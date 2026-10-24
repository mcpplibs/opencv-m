#!/usr/bin/env python3
"""Check explicit pinned-header review coverage without hiding public gaps."""
import argparse
from collections import Counter
import json
from pathlib import Path
import re
import tomllib

ROOT = Path(__file__).resolve().parents[1]
DECISIONS = {"cuda-sdk", "public-gap", "runtime-internal", "forwarder", "platform-sdk",
             "parallel-sdk", "upstream-internal", "implementation", "textual-only",
             "binding-only", "implemented", "typed-mapped", "documentation"}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--inventory", type=Path, default=ROOT / "target/api-audit/inventory.json")
    parser.add_argument("--require-complete", action="store_true",
                        help="fail while reviewed public API gaps remain; use for full-support acceptance")
    args = parser.parse_args()
    review = tomllib.loads((ROOT / "port/api/header-review.toml").read_text())
    assert review["schema"] == 1 and review["upstream-version"] == "5.0.0"
    inventory = json.loads(args.inventory.read_text())
    assert inventory["opencv"] == review["upstream-version"]
    records, counts, gaps = {}, Counter(), []
    for group in review["groups"]:
        decision = group["decision"]
        assert decision in DECISIONS and group["reason"].strip(), group
        for header in group["headers"]:
            assert header not in records, f"Duplicate review: {header}"
            assert header in inventory["headers"], f"Unknown review: {header}"
            path = ROOT / "upstream/opencv" / header
            assert path.is_file(), header
            records[header] = decision
            counts[decision] += 1
            if decision == "public-gap":
                gaps.append(header)
            if decision == "implemented":
                assert inventory["headers"][header] == "loaded", f"Implemented facade header not loaded: {header}"
            if decision == "forwarder":
                source = re.sub(r"/\*.*?\*/|//[^\n]*", "", path.read_text(), flags=re.S)
                assert "#include" in source and all(
                    not line.strip() or line.lstrip().startswith("#") for line in source.splitlines()), header
    missing = sorted(header for header, status in inventory["headers"].items()
                     if status == "not-in-facade" and header not in records)
    assert not missing, f"Unreviewed headers: {missing}"
    print("Reviewed header decisions:", json.dumps(dict(sorted(counts.items()))))
    print(f"Remaining public-gap records: {len(gaps)}; review coverage is not implementation coverage")
    if args.require_complete and gaps:
        raise SystemExit("Full-support acceptance still has public gaps:\n" + "\n".join(gaps))


if __name__ == "__main__":
    main()
