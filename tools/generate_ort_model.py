#!/usr/bin/env python3
"""Generate a small ONNX Add model, IR8/opset13, float[1,3] -> float[1,3]."""
import argparse
from pathlib import Path


def varint(value):
    out = bytearray()
    while value > 127:
        out.append((value & 127) | 128)
        value >>= 7
    out.append(value)
    return bytes(out)


def field(number, value):
    if isinstance(value, int):
        return varint(number << 3) + varint(value)
    if isinstance(value, str):
        value = value.encode()
    return varint((number << 3) | 2) + varint(len(value)) + value


def model():
    shape = field(1, field(1, 1)) + field(1, field(1, 3))
    tensor_type = field(1, 1) + field(2, shape)
    def value_info(name):
        return field(1, name) + field(2, field(1, tensor_type))
    node = field(1, "x") + field(1, "x") + field(2, "y") + field(3, "double_input") + field(4, "Add")
    graph = field(1, node) + field(2, "mwrap_opencv_ort")
    graph += field(11, value_info("x")) + field(12, value_info("y"))
    return field(1, 8) + field(2, "mwrap-fixture") + field(7, graph) + field(8, field(2, 13))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    content = model()
    header = "#pragma once\ninline constexpr unsigned char opencv_ort_model[] = {"
    header += ", ".join(f"0x{x:02x}" for x in content) + "};\n"
    outputs = {root / "tests/fixtures/ort_add.onnx": content,
               root / "tests/fixtures/ort_model.hpp": header.encode(),
               root / "templates/onnxruntime/src/model.hpp": header.encode()}
    for path, data in outputs.items():
        if args.check:
            assert path.read_bytes() == data, str(path)
        else:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
