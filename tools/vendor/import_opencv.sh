#!/usr/bin/env bash
# Re-import the pinned OpenCV source into upstream/opencv/.
# Host-independent: with no argument it downloads the pinned official tarball
# itself (into a gitignored work dir) and verifies its sha256 before pruning.
# Usage: tools/vendor/import_opencv.sh [path-to-extracted-opencv-5.0.0]
set -euo pipefail
VER="5.0.0"
PIN_SHA="b0528f5a1d379d59d4701cb28c36e22214cc51cf64594e5b56f2d3e6c0233095"
URL_GLOBAL="https://github.com/opencv/opencv/archive/refs/tags/${VER}.tar.gz"
URL_CN="https://gitcode.com/mcpp-res/opencv/releases/download/${VER}/opencv-${VER}.tar.gz"
ROOT="$(git rev-parse --show-toplevel)"
DST="$ROOT/upstream/opencv"

WORK="$ROOT/target/vendor-import"
mkdir -p "$WORK"
TAR="$WORK/opencv-${VER}.tar.gz"
if ! echo "$PIN_SHA  $TAR" | sha256sum -c - 2>/dev/null; then
    URL="$URL_GLOBAL"; [ "${MCPP_MIRROR:-}" = "CN" ] && URL="$URL_CN"
    echo "downloading $URL"
    curl -fL --retry 3 -o "$TAR" "$URL"
    echo "$PIN_SHA  $TAR" | sha256sum -c -
fi
SRC="${1:-}"
if [ -z "$SRC" ]; then
    rm -rf "$WORK/opencv-${VER}"
    tar -xzf "$TAR" -C "$WORK"
    SRC="$WORK/opencv-${VER}"
fi
[ -f "$SRC/CMakeLists.txt" ] || { echo "not an opencv tree: $SRC" >&2; exit 1; }
# A caller-supplied tree must also match the pinned archive, not merely carry
# the expected directory/version name.
python3 "$ROOT/tools/check_upstream.py" --archive "$TAR" --root "$SRC"
STAGE=$(mktemp -d "$WORK/import.XXXXXX")
trap 'rm -rf "$STAGE"' EXIT
NEW="$STAGE/opencv"
mkdir -p "$NEW"
KEEP_MODULES="core imgproc imgcodecs highgui videoio dnn flann geometry calib features objdetect photo ptcloud stereo stitching video"
KEEP_3RDPARTY="libjpeg-turbo libpng zlib protobuf mlas flatbuffers dlpack"
cp "$SRC"/{LICENSE,COPYRIGHT,README.md} "$NEW/"
cp -r "$SRC/include" "$NEW/include"
mkdir -p "$NEW/modules" "$NEW/3rdparty" "$NEW/modules/python"
for m in $KEEP_MODULES; do cp -r "$SRC/modules/$m" "$NEW/modules/$m"; done
cp -r "$SRC/modules/python/src2" "$NEW/modules/python/src2"
for t in $KEEP_3RDPARTY; do cp -r "$SRC/3rdparty/$t" "$NEW/3rdparty/$t"; done
cp "$SRC/3rdparty/readme.txt" "$NEW/3rdparty/"
# Slim the kept modules: test/perf trees and non-C++ binding misc dirs.
for m in $KEEP_MODULES; do
    rm -rf "$NEW/modules/$m"/{test,perf}
    for b in java objc python js; do rm -rf "$NEW/modules/$m/misc/$b"; done
done
du -sh "$NEW"
mkdir -p "$ROOT/upstream"
if [ -d "$DST" ]; then mv "$DST" "$STAGE/previous"; fi
if ! mv "$NEW" "$DST"; then
    if [ -d "$STAGE/previous" ]; then mv "$STAGE/previous" "$DST"; fi
    exit 1
fi
