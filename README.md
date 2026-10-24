# opencv

> OpenCV 5.0.0 through C++23 modules; the upstream cv:: API is preserved.

**English** · [简体中文](README.zh.md) · [Upstream](https://github.com/opencv/opencv) · [Releases](https://github.com/mcpplibs/opencv-m/releases) · [mcpp](https://github.com/mcpp-community/mcpp) · [Index](https://github.com/mcpplibs/mcpp-index)

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![License](https://img.shields.io/badge/license-Apache-2.0-blue.svg)](LICENSE)
[![CI](https://github.com/mcpplibs/opencv-m/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/mcpplibs/opencv-m/actions/workflows/ci.yml)

Sixteen main modules, optional native windows, codecs, video and ORT CPU; pinned upstream sources are separate from adapters.

## Modules

Optional modules require their corresponding mcpp features.

- `import mcpplibs.mwrap.opencv;`: Enabled module aggregate
- `import mcpplibs.mwrap.opencv.core;`: Mat, types and basic operations
- `import mcpplibs.mwrap.opencv.imgproc;`: Image processing and drawing
- `import mcpplibs.mwrap.opencv.imgcodecs;`: Image loading and saving
- `import mcpplibs.mwrap.opencv.highgui;`: Window API; select a backend feature
- `import mcpplibs.mwrap.opencv.videoio;`: Video input/output
- `import mcpplibs.mwrap.opencv.dnn;`: Neural network inference

> More Modules API -> [Usage and API](docs/usage.md)

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
