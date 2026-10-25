#!/usr/bin/env python3
"""Verify every retained upstream file against the checksum-pinned archive."""
import argparse
import hashlib
from pathlib import Path
import tarfile
import tomllib


def main():
    root = Path(__file__).resolve().parents[1]
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--archive", type=Path, default=root / "target/vendor-import/opencv-5.0.0.tar.gz")
    parser.add_argument("--root", type=Path, default=root / "upstream/opencv")
    args = parser.parse_args()
    pin = tomllib.loads((root / "upstream/sources.lock.toml").read_text())["opencv"]
    with args.archive.open("rb") as stream:
        actual = hashlib.file_digest(stream, "sha256").hexdigest()
    assert actual == pin["sha256"], (actual, pin["sha256"])
    count = 0
    with tarfile.open(args.archive) as archive:
        for file in sorted(args.root.rglob("*")):
            if not file.is_file() or "__pycache__" in file.parts:
                continue
            relative = file.relative_to(args.root)
            member = archive.extractfile("opencv-" + pin["version"] + "/" + relative.as_posix())
            assert member is not None and member.read() == file.read_bytes(), str(relative)
            count += 1
    print(f"Verified {count} retained files against OpenCV {pin['version']} archive SHA256 {actual}")


if __name__ == "__main__":
    main()
