#!/usr/bin/env python3
"""Exercise the compiled build program with LF and Windows CRLF checkouts."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("program", type=Path, help="compiled build.mcpp executable")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    program = args.program.resolve()
    results = []
    for ending in (b"\n", b"\r\n"):
        with tempfile.TemporaryDirectory(prefix="opencv-config-") as directory:
            fixture = Path(directory)
            inputs = [root / "src/selected.cppm.in", root / "port/config/modules.txt",
                      root / "generated/linux/cvconfig.h", root / "generated/linux/INCLUDE_DIRS.txt"]
            inputs += sorted((root / "port/config/sources/linux").glob("*.txt"))
            for source in inputs:
                dest = fixture / source.relative_to(root)
                dest.parent.mkdir(parents=True, exist_ok=True)
                dest.write_bytes(source.read_bytes().replace(b"\r\n", b"\n").replace(b"\n", ending))
            out = fixture / "out"
            out.mkdir()
            env = {key: value for key, value in os.environ.items() if not key.startswith("MCPP_FEATURE_")}
            env.update(MCPP_MANIFEST_DIR=str(fixture), MCPP_OUT_DIR=str(out), MCPP_TARGET_OS="linux",
                       MCPP_FEATURE_IMGPROC="1", MCPP_FEATURE_GEOMETRY="1", MCPP_FEATURE_FLANN="1")
            result = subprocess.run([str(program)], env=env, check=True, capture_output=True)
            selected = (out / "selected.cppm").read_text()
            assert "export import mcpplibs.mwrap.opencv.imgproc;" in selected, selected
            assert "export import mcpplibs.mwrap.opencv.imgcodecs;" not in selected, selected
            config = (out / "cvconfig.h").read_text()
            assert "#define HAVE_JPEG" not in config, config
            assert "#define HAVE_PNG" not in config, config
            # Compare source directives and generated configuration, normalizing
            # only the temporary output path, which necessarily differs.
            directives = result.stdout.decode().replace(str(out), "<out>")
            results.append((selected, config, directives))
    assert results[0] == results[1], "LF and CRLF checkouts selected different build inputs"
    print("Build configuration: LF and CRLF select identical modules, sources and codec configuration")


if __name__ == "__main__":
    main()
