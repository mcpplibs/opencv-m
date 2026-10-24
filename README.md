# opencv-m

OpenCV **5.0.0** through C++23 modules. Keep `cv::Mat`, `cv::cvtColor` and the upstream API.

[简体中文](README.zh.md)

## Quick start

Use mcpp **2026.10.24.1**. The version below is a candidate until its index entry is published.

```sh
mcpp new vision --template mcpplibs.mwrap.opencv@5.0.0.1
cd vision
mcpp run
```

The default template generates an image, converts it to grayscale and writes a PNG.

## Existing projects

```toml
[dependencies]
"mcpplibs.mwrap.opencv" = { version = "5.0.0.1", features = ["imgproc", "imgcodecs-png"] }
```

```cpp
import mcpplibs.mwrap.opencv;
namespace macros = mcpplibs::mwrap::opencv::macros;
int main() {
    cv::Mat image(64, 64, macros::CV_8UC3, cv::Scalar(20, 80, 160));
    return image.empty();
}
```

## Scope

Core is always available. `all-modules` selects sixteen main modules; codecs and
backends are explicit features. Module availability does not imply complete API or hardware coverage.

- `highgui-native`: GTK3 on Linux x86_64 glibc, Win32 on Windows, Cocoa on macOS.
- `videoio-native`: V4L2, Media Foundation or AVFoundation; `videoio-ffmpeg` adds FFmpeg.
- `dnn-onnxruntime`: OpenCV DNN with the ORT CPU SDK on its supported targets.
- `simd`: [native CPU intrinsics](docs/simd.md) through `mcpplibs.mwrap.opencv.core.simd`.

Named templates: `:window`, `:video`, `:onnxruntime`. The window template needs a desktop session.
For preprocessing macros, include `<mwrap/opencv/macros.hpp>` before imports.

## Documentation

[Usage and features](docs/usage.md) · [Architecture](docs/architecture.en.md) ·
[Examples](examples) · [Readiness](.agents/docs/2026-10-24-readiness-audit.md) · [Apache-2.0](LICENSE)
