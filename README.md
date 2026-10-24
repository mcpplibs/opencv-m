# opencv

> OpenCV 5.0.0 through C++23 modules; the upstream cv:: API is preserved.
> Sixteen main modules, optional native windows, codecs, video and ORT CPU; pinned upstream sources are separate from adapters.

**English** · [简体中文](README.zh.md)

Upstream: [opencv/opencv](https://github.com/opencv/opencv)

[Releases](https://github.com/mcpplibs/opencv-m/releases) · [mcpp](https://github.com/mcpp-community/mcpp) · [Index](https://github.com/mcpplibs/mcpp-index)

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![License](https://img.shields.io/badge/license-Apache-2.0-blue.svg)](LICENSE)
[![CI](https://github.com/mcpplibs/opencv-m/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/mcpplibs/opencv-m/actions/workflows/ci.yml)

## Getting started

```sh
mcpp new vision --template mcpplibs.mwrap.opencv@5.0.0
cd vision
mcpp run
```

```toml
[dependencies]
mcpplibs.mwrap.opencv = { version = "5.0.0", features = ["imgproc", "imgcodecs-png"] }
```

```cpp
import mcpplibs.mwrap.opencv;
namespace macros = mcpplibs::mwrap::opencv::macros;
int main() { return cv::Mat(8, 8, macros::CV_8UC3).empty(); }
```

## Modules

The root aggregates enabled modules. Import `.core`, `.imgproc`, `.dnn`, etc. directly as needed.
See the [sixteen-module and API matrices](docs/coverage.md).

## Features

Core is mandatory; `default=[]`. Select capabilities explicitly.

| Feature | Purpose |
|---|---|
| `all-modules` | Sixteen main modules; backends stay explicit |
| `highgui-native` | GTK3 / Win32 / Cocoa windows |
| `imgcodecs-png`, `imgcodecs-jpeg`, `imgcodecs-webp` | Selected image formats |
| `videoio-native`, `videoio-ffmpeg` | Native platform / FFmpeg video |
| `dnn-onnxruntime` | ORT CPU inference |
| `simd`, `simd-emulated` | Native baseline / software vectors |

Macro mappings use `.macros`; include `<mwrap/opencv/macros.hpp>` before imports for preprocessing macros.

## Examples

The default template generates its own image. Named templates: `:window`, `:video`, `:onnxruntime`.
[Examples](examples) · [Usage](docs/usage.md)

## Build and toolchains

Tested with Linux GCC/LLVM, Windows LLVM and macOS ARM64 LLVM. Window templates need a desktop session.
Physical cameras require devices; GPU/contrib and nonbaseline ISA are outside this release.
[Architecture](docs/architecture.en.md) · [Coverage and compiler limits](docs/coverage.md)

## License

[Apache-2.0](LICENSE); upstream and selected dependencies retain their notices.
