#!/usr/bin/env python3
"""Verify host-independent ordering, quoted-text exclusion and original bytes."""
from pathlib import Path
import tempfile

import generate_source_notices as generator


class WindowsOrderedPath(type(Path())):
    def __lt__(self, other):
        return tuple(part.casefold() for part in self.parts) < tuple(part.casefold() for part in other.parts)


with tempfile.TemporaryDirectory(prefix="opencv-notice-order-") as directory:
    sdk = Path(directory) / "upstream/opencv"
    sdk.mkdir(parents=True)
    first = b"/* Copyright Z\r\n * original CRLF conditions\r\n */"
    second = b"// Copyright a\n// original notice"
    (sdk / "Z.h").write_bytes(first + b"\nint z;\n")
    (sdk / "a.h").write_bytes(second + b"\nint a;\n")
    (sdk / "strings.hpp").write_bytes(
        b'const char* ordinary = "// Copyright string only";\n'
        b'const char* raw = R"tag(/* License string only */)tag";\n')
    generator.ROOT = Path(directory)
    native, unique, locations = generator.render()
    generator.ROOT = WindowsOrderedPath(directory)
    emulated, other_unique, other_locations = generator.render()
    assert native == emulated, "Host path comparison changed notice bytes/order"
    assert unique == other_unique == 2 and locations == other_locations == 2
    assert native.index(first) < native.index(second), "Expected canonical case-sensitive order"
    assert first in native and second in native, "Original comment bytes must survive"
    assert b"string only" not in native, "Quoted strings are not source notices"
print("Notice ordering matches across host path rules; original bytes and quoted-text exclusion pass")
