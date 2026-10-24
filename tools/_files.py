"""Preserve timestamps of unchanged generated build inputs."""
from pathlib import Path


def write_if_changed(path: Path, text: str):
    if path.exists() and path.read_text(encoding="utf-8") == text:
        return
    path.write_text(text, encoding="utf-8", newline="\n")
