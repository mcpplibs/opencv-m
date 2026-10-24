#!/usr/bin/env python3
"""Validate exact source-reviewed rejections from the import-only API probe.

Coverage of reviewed rejections is not overload or runtime completeness.
"""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import tomllib

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--inventory", type=Path, default=ROOT / "target/api-audit/inventory.json")
    parser.add_argument("--probe", type=Path, default=ROOT / "target/api-audit/unlisted-import-probe/report.json")
    parser.add_argument("--require-complete", action="store_true", help="also reject explicit public-gap decisions")
    args = parser.parse_args()
    review = tomllib.loads((ROOT / "port/api/entity-review.toml").read_text())
    inventory = json.loads(args.inventory.read_text())
    probe = json.loads(args.probe.read_text())
    assert review["schema"] == 1 and review["upstream-version"] == inventory["opencv"]
    assert probe["profile"] == inventory["profile"], "Inventory/probe profiles differ"
    features = set(probe["profile"]["features"])
    assert "all-modules" in features, "Review the complete main-module inventory"
    rejected = set(probe["not-visible"])
    recorded, active, counts, gaps = set(), set(), Counter(), []
    for group in review["groups"]:
        assert group["decision"] in {"upstream-internal", "binding-only", "cuda-sdk", "public-gap"}, group
        assert group["reason"].strip() and group["evidence"] and group["names"], group
        source = ROOT / "upstream/opencv" / group["source"]
        assert source.resolve().is_relative_to((ROOT / "upstream/opencv").resolve()), source
        data = source.read_bytes()
        assert hashlib.sha256(data).hexdigest() == group["source-sha256"], f"Re-review changed SDK source: {source}"
        assert all(anchor in data.decode() for anchor in group["evidence"]), source
        names = set(group["names"])
        assert len(names) == len(group["names"]) and not names & recorded, "Duplicate reviewed name"
        recorded.update(names)
        if not set(group.get("features", [])) <= features:
            continue
        for name in names:
            assert name in inventory["entities"], f"Reviewed name absent from active inventory: {name}"
            assert group["source"] in inventory["entities"][name]["headers"], name
        assert names <= rejected, f"Remove stale exclusions for newly visible names: {sorted(names - rejected)}"
        active.update(names)
        counts[group["decision"]] += len(names)
        if group["decision"] == "public-gap":
            gaps.extend(names)
    assert rejected == active, f"Unreviewed rejected names: {sorted(rejected - active)}"
    print(f"Source-reviewed rejected names: {len(active)}; decisions: {json.dumps(dict(sorted(counts.items())))}")
    print("Each rejection has exact names, a reason, pinned source hash and literal source evidence")
    print("This check does not certify visible overloads, runtime backends or other-ISA header coverage")
    if args.require_complete and gaps:
        raise SystemExit("Unfinished public entity decisions:\n" + "\n".join(sorted(gaps)))


if __name__ == "__main__":
    main()
