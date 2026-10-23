# opencv-m

OpenCV 5.0.0 C++ APIs through `mcpplibs.mwrap.opencv`, preserving `cv::` names.
The candidate package is **5.0.0.1**: upstream 5.0.0 plus wrapper revision 1.

**English** · [中文](README.zh.md)

This branch is an implementation checkpoint. All sixteen main modules are integrated and have passed local Linux GCC/LLVM tests.
The native GTK3 window backend has passed local Linux tests and Cocoa passed
native macOS CI; Win32 validation is pending. Public API coverage auditing and the ONNX Runtime bridge are
still being implemented. Passing the current CI does not establish full OpenCV support.

## Build the checkout

Use mcpp **2026.10.21.1**, pinned in .xlings.json and CI:

```sh
mcpp test
mcpp test --features imgcodecs-png,imgcodecs-jpeg
mcpp test --features all-modules --strict
mcpp test --features dnn,unifont
python3 tools/test_template.py
```

Linux GCC/LLVM, Linux musl static, macOS arm64 and Windows MSVC-ABI builds are
covered in CI. Core and PNG/JPEG behavior run on all five legs; the DNN/unifont
combination currently runs on Linux GCC. Hardware capabilities require their
own runtime evidence.

## Project template

The default template generates its own image, converts it to grayscale and
writes out.png. Its actual mcpp renderer, generated manifest, build and execution
are tested. No input image is required.

After this candidate is published and indexed:

```sh
mcpp new myvision --template mcpplibs.mwrap.opencv@5.0.0.1
cd myvision
mcpp run
```

The named `window` template displays a generated image and processes window events:

```sh
mcpp new mywindow --template mcpplibs.mwrap.opencv@5.0.0.1:window
cd mywindow
mcpp run
```

Linux requires a graphical session and the `pkg-config` executable. The GTK3
headers and libraries come from declared SDK dependencies. Headless CI uses Xvfb
and `mcpp run -- --smoke-test` to close the window automatically.

The installed-template fixture used during development stages candidate sources;
it is separate from the still-required published archive/index installation test.

## Dependencies and features

```toml
[dependencies."mcpplibs.mwrap"]
opencv = { version = "5.0.0.1", features = ["imgcodecs-png", "imgcodecs-jpeg"] }
```

`default = []`; core is mandatory. Optional modules add their upstream dependency
closure. Consumers can disable a dependency's defaults with `default-features = false`.

| Feature | Current behavior |
|---|---|
| imgproc, flann, geometry | Selected algorithm modules |
| calib, features, objdetect, photo, ptcloud, stereo, stitching, video | Remaining upstream main C++ modules |
| all-modules | All sixteen main modules; external codecs, GUI backends and SDKs stay explicit |
| imgcodecs | Image codec API and common source set |
| imgcodecs-png, imgcodecs-jpeg | Enable the corresponding codec and imply imgcodecs |
| videoio | Video I/O API |
| videoio-ffmpeg | Adds the FFmpeg implementation and compat.ffmpeg dependency |
| videoio-v4l2 | Adds V4L2 on Linux; other targets are refused |
| highgui | GUI API only; current window backend is NONE |
| highgui-native | Select GTK3 on Linux x86_64 glibc, Win32 on Windows, or Cocoa on macOS |
| highgui-gtk3, highgui-win32, highgui-cocoa | Explicit native backend; mismatched targets are refused |
| dnn | OpenCV's existing native DNN implementation; does not enable ORT |
| unifont | Unicode/CJK font rendering, implying imgproc |

There is no current full/all-modules promise. GTK3 window creation, image display,
event processing and destruction have passed locally; Cocoa passed native CI.
Win32 validation is pending. dnn-onnxruntime remains planned. Calling imshow with only the API-only
highgui feature still reports missing window backend support; enable a backend.

## Modules, API and macros

```cpp
import mcpplibs.mwrap.opencv;
// Or import the selected module directly:
// import mcpplibs.mwrap.opencv.imgproc;
```

The root re-exports core, macros and the selected optional modules. The aggregate
selection module is generated into the configuration-specific build directory
before scanning, with direct imports. Optional modules are not exposed merely
because their source exists in the checkout.

`cv::Mat`, `cv::cvtColor` and other upstream APIs keep their spelling. Typed macro
mappings use a separate namespace:

```cpp
namespace macros = mcpplibs::mwrap::opencv::macros;
cv::Mat image(64, 64, macros::CV_8UC3, cv::Scalar(20, 80, 160));
```

For preprocessing macros such as CV_Assert, include
`<mwrap/opencv/macros.hpp>` **before** importing. Raw macros still expand inside
qualified names: use raw macro and typed-mapping forms in separate contexts.

## Repository structure and upgrades

- upstream/opencv contains the pinned source; upstream/sources.lock.toml records provenance and archive SHA256.
- port contains configuration inputs and compatibility headers.
- src contains wrapper interfaces and adapters; generated contains stable snapshots.
- build.mcpp writes feature-specific configuration into its own output directory.
- tools/generate_build.py regenerates the manifest, dependency map and source catalogs.
- tools/check_upstream.py verifies retained file bytes against the pinned upstream archive.
- templates and tests contain executable consumer examples and behavior checks.

The import includes all sixteen main C++ modules. Newly integrated SIFT and
pyramidal optical-flow dispatch use baseline implementations; FAST retains its
x86 AVX2 TU. OpenCL kernels for the eight new modules are not integrated and
configuration explicitly refuses enabling them. API completeness, additional
backends and installed archives remain release gates. Design and
implementation evidence live under .agents/docs; the common mwrap contract is
maintained in mcpp-index/docs/mwrap.md.

## License

The root LICENSE is Apache-2.0 and the package metadata follows it. The vendored
OpenCV sources are Apache-2.0; third-party components retain their own licenses.
