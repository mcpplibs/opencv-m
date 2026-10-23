#!/usr/bin/env python3
"""Generate the mcpp manifest and feature source catalogs from pinned build data."""
import copy
import glob
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CONFIG = ROOT / "port/config"


def expand_braces(pattern):
    match = re.search(r"\{([^{}]+)\}", pattern)
    if not match:
        return [pattern]
    return [expanded for value in match.group(1).split(",")
            for expanded in expand_braces(pattern[:match.start()] + value + pattern[match.end():])]


def expand(pattern):
    paths = {Path(path).relative_to(ROOT).as_posix()
             for candidate in expand_braces(pattern)
             for path in glob.glob(str(ROOT / candidate), recursive=True) if Path(path).is_file()}
    if not paths:
        raise ValueError(f"Source pattern matched no files: {pattern}")
    return sorted(paths)


def source_group(path):
    if any(name in path for name in ("/libjpeg-turbo/", "/jpeg12/", "/jpeg16/")):
        return "imgcodecs-jpeg"
    if "/libpng/" in path:
        return "imgcodecs-png"
    if "/zlib/" in path:
        return "core"
    if "cap_ffmpeg" in path:
        return "videoio-ffmpeg"
    if "cap_v4l" in path:
        return "videoio-v4l2"
    match = re.search(r"/(?:modules|tu/w)/([^/]+)/", path)
    if match:
        return match.group(1)
    raise ValueError(f"Unclassified source: {path}")


def module_dependencies(name):
    cmake = (ROOT / "upstream/opencv/modules" / name / "CMakeLists.txt").read_text()
    match = re.search(r"ocv_(?:define|add)_module\s*\((.*?)\)", cmake, re.S)
    if not match:
        raise ValueError(f"No upstream module definition for {name}")
    required = re.split(r"\b(?:OPTIONAL|WRAP|PRIVATE_REQUIRED|PRIVATE_OPTIONAL)\b", match.group(1))[0]
    return sorted(set(re.findall(r"\bopencv_([a-z0-9_]+)\b", required)) - {"core", name})


def scalar(value):
    if isinstance(value, str):
        return json.dumps(value, ensure_ascii=False)
    if isinstance(value, bool):
        return "true" if value else "false"
    if isinstance(value, list):
        return "[" + ", ".join(scalar(item) for item in value) + "]"
    return str(value)


def key(value):
    return value if re.fullmatch(r"[A-Za-z0-9_-]+", value) else scalar(value)


def tables(data, prefix=(), array=False):
    output = []
    if prefix:
        joined = ".".join(key(part) for part in prefix)
        output.append(("[[" + joined + "]]" if array else "[" + joined + "]") + "\n")
    for name, value in data.items():
        if not isinstance(value, dict) and not (isinstance(value, list) and value and isinstance(value[0], dict)):
            output.append(f"{key(name)} = {scalar(value)}\n")
    output.append("\n")
    for name, value in data.items():
        if isinstance(value, dict):
            output += tables(value, prefix + (name,))
        elif isinstance(value, list) and value and isinstance(value[0], dict):
            for item in value:
                output += tables(item, prefix + (name,), array=True)
    return output


def generate():
    config = json.loads((CONFIG / "platforms.json").read_text())
    modules = sorted(p.name for p in (ROOT / "upstream/opencv/modules").iterdir()
                     if p.is_dir() and (ROOT / "src" / (p.name + ".cppm")).exists())
    (CONFIG / "modules.txt").write_text("\n".join(modules) + "\n")
    dependencies = {name: module_dependencies(name) for name in modules}
    (CONFIG / "module-dependencies.json").write_text(json.dumps(dependencies, indent=2) + "\n")
    targets = copy.deepcopy(config["target"])
    added_modules = ["calib", "features", "objdetect", "photo", "ptcloud", "stereo", "stitching", "video"]
    for predicate, target in targets.items():
        build = target.get("build", {})
        if "sources" in build:
            os_name = {"cfg(linux)": "linux", "cfg(macos)": "macosx", "cfg(windows)": "windows"}[predicate]
            groups = {}
            for pattern in build.pop("sources"):
                for path in expand(pattern):
                    groups.setdefault(source_group(path), set()).add(path)
            # Upstream's default module source glob is recursive. Keep all
            # implementation subdirectories, including barcode/aruco/ccm and
            # tracking; architecture-specific FAST is a separate x86 TU.
            for name in added_modules:
                sources = (ROOT / "upstream/opencv/modules" / name / "src").rglob("*.cpp")
                groups[name] = {p.relative_to(ROOT).as_posix() for p in sources
                                if not re.search(r"\.(?:avx\w*|sse\w*|neon)\.cpp$", p.name)}
            if os_name in ("linux", "windows"):
                groups["features"].add("upstream/opencv/modules/features/src/fast.avx2.cpp")
                build.setdefault("flags", []).append({
                    "glob": "**/features/src/fast.avx2.cpp", "cxxflags": ["-mavx2"],
                    "defines": ["CV_CPU_COMPILE_AVX2=1", "CV_CPU_DISPATCH_MODE=AVX2"]})
            directory = CONFIG / "sources" / os_name
            directory.mkdir(parents=True, exist_ok=True)
            for group, paths in sorted(groups.items()):
                (directory / (group + ".txt")).write_text("\n".join(sorted(paths)) + "\n")
        for field in ("cflags", "cxxflags"):
            if field in build:
                build[field] = [flag for flag in build[field] if flag != "-DHAVE_CAMV4L2"]
        for flags in build.get("flags", []):
            if "defines" in flags:
                flags["defines"] = [value for value in flags["defines"]
                                    if value not in {"HAVE_FFMPEG", "HAVE_FFMPEG_LIBAVDEVICE"}]
    features = {"default": []}
    for codec in ("zlib", "libpng", "libjpeg-turbo"):
        targets["cfg(unix)"]["build"]["flags"].append({
            "glob": f"**/3rdparty/{codec}/**",
            "cflags": ["-fvisibility=hidden"], "cxxflags": ["-fvisibility=hidden"]})
    for bits in (12, 16):
        targets["cfg(unix)"]["build"]["flags"].append({
            "glob": f"**/tu/jpeg{bits}/**",
            "cflags": ["-fvisibility=hidden"], "cxxflags": ["-fvisibility=hidden"]})
    for name in modules:
        if name == "core":
            continue
        features[name] = {"implies": dependencies[name], "sources": [f"src/{name}.cppm"],
                          "defines": ["HAVE_OPENCV_" + name.upper()]}
    features["all-modules"] = {"implies": [name for name in modules if name != "core"]}
    # Retain the upstream DNN-specific TU flags, but gate all sources through
    # the module feature and the per-target source catalogs.
    if "dnn" in features:
        features["dnn"].update(copy.deepcopy(config["dnn"]))
        features["dnn"]["implies"] = dependencies["dnn"]
    features.update({
        "imgcodecs-jpeg": {"implies": ["imgcodecs"]},
        "imgcodecs-png": {"implies": ["imgcodecs"]},
        "videoio-ffmpeg": {"implies": ["videoio"], "defines": ["HAVE_FFMPEG", "HAVE_FFMPEG_LIBAVDEVICE"]},
        "videoio-v4l2": {"implies": ["videoio"], "defines": ["HAVE_CAMV4L2"]},
        "unifont": {"implies": ["imgproc"], "defines": ["HAVE_UNIFONT"]},
        "highgui-native": {"implies": ["highgui", "imgcodecs"]},
        "highgui-gtk3": {"implies": ["highgui", "imgcodecs"]},
        "highgui-win32": {"implies": ["highgui"]},
        "highgui-cocoa": {"implies": ["highgui"]},
    })
    # GTK's pthread compiler mode is private to its implementation TU. Baking
    # it into a public BMI would require every consumer to use -pthread too.
    for name in ("highgui-native", "highgui-gtk3"):
        features[name]["flags"] = [{"glob": "**/highgui/src/window_gtk.cpp", "cxxflags": ["-pthread"]}]
    gtk_target = targets.setdefault('cfg(all(os = "linux", env = "gnu", arch = "x86_64"))', {})
    gtk_target["feature-xlings"] = {
        name: {"xim:gtk3": "3.24.43", "xim:libXtst": "1.2.5"}
        for name in ("highgui-native", "highgui-gtk3")}
    build = copy.deepcopy(config["build"])
    for name in added_modules:
        build["include_dirs"].append(f"upstream/opencv/modules/{name}/include")
        # These are upstream implementation flags, never public BMI flags.
        targets["cfg(unix)"]["build"]["flags"].append({
            "glob": f"**/modules/{name}/**", "defines": ["__OPENCV_BUILD=1", "NDEBUG"]})
        targets["cfg(windows)"]["build"]["flags"].append({
            "glob": f"**/modules/{name}/**", "defines": ["__OPENCV_BUILD=1", "NDEBUG"]})
    # The bundled zlib is an implementation detail, not another provider of
    # the public zlib ABI used by GUI SDKs and other dependencies.
    build.setdefault("defines", []).append("Z_PREFIX")
    # build.mcpp emits the selected interface with concrete imports after
    # feature resolution, before mcpp scans the generated module.
    build["sources"] = ["src/opencv.cppm", "src/core.cppm", "src/macros.cppm"]
    manifest = {
        "package": {"namespace": "mcpplibs.mwrap", "name": "opencv", "version": "5.0.0.1",
                    "description": "OpenCV 5.0.0 public C++ APIs through optional mwrap modules",
                    "license": "Apache-2.0", "repo": "https://github.com/mcpplibs/opencv-m",
                    "metadata": {"mwrap": {"upstream-version": "5.0.0", "wrapper-revision": 1}}},
        "build": build, "target": targets,
        "dev-dependencies": {"compat": {"gtest": "1.15.2"}},
        "features": features,
        "feature-deps": {"videoio-ffmpeg": {"compat": {"ffmpeg": "8.1.2"}}},
        "targets": {"opencv": {"kind": "lib"}},
    }
    (ROOT / "mcpp.toml").write_text(
        ("# Generated by tools/generate_build.py; edit port/config inputs.\n" + "".join(tables(manifest))).rstrip() + "\n")
    # The installed header contains only the mandatory module; optional
    # availability names come from the selected package features.
    (ROOT / "generated/common/opencv2/opencv_modules.hpp").write_text(
        "#pragma once\n#define HAVE_OPENCV_CORE\n")


if __name__ == "__main__":
    generate()
