Detailed usage and background. See the [README](../README.md) for the current candidate quickstart and support boundary.

# opencv-m

OpenCV 5.0.0 C++ APIs through `mcpplibs.mwrap.opencv`, preserving `cv::` names.
The candidate package is **5.0.0.1**: upstream 5.0.0 plus wrapper revision 1.

**English** · [中文](../README.zh.md)

This is an unpublished candidate with all sixteen main modules, explicit native
window/video backends, PNG/JPEG/WebP and the ORT CPU bridge. Module availability,
API coverage, runtime backends and target certification are separate questions.
See the [coverage matrices](coverage.md) and [readiness ledger](../.agents/docs/2026-10-24-readiness-audit.md)
for exact evidence and outstanding gates. Current source/index CI and final
archive installation/deployment must pass before release.

## Build the checkout

Use mcpp **2026.10.24.1**, pinned in .xlings.json and CI:

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

The named `onnxruntime` template runs a generated ONNX Add model through
OpenCV's ENGINE_ORT and also imports the standalone ORT module:

```sh
mcpp new inference --template mcpplibs.mwrap.opencv@5.0.0.1:onnxruntime
cd inference
mcpp run
```

It declares the Linux toolchain-coupled C++ runtime needed by the shared SDK.
The bridge's tests require ORT's real CPU session profile, so a native-engine
fallback cannot satisfy them. Enabling dnn-onnxruntime also re-exports the ORT
module through the selected aggregate, keeping cross-package BMI paths consistent
for GCC. `cv::` and `Ort::` retain their upstream names.

Local-source template tests and actual immutable-archive installation tests are
separate. The index acceptance uses a fresh private home, runs the unmodified
template and indexed API consumer, then packs and relocates the application.
A successful older archive does not certify a later source checkpoint.

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
| imgcodecs-webp | Adds libwebp encode/decode, mux and demux for lossless/alpha images, animation and metadata |
| videoio | Video I/O API |
| videoio-ffmpeg | Adds the FFmpeg implementation and compat.ffmpeg dependency |
| videoio-v4l2 | Adds V4L2 on Linux; other targets are refused |
| videoio-native | Select V4L2 on Linux, Media Foundation on Windows, or AVFoundation on macOS |
| videoio-msmf / videoio-avfoundation | Explicit Windows MSVC ABI / macOS native video backends; mismatched targets are refused |
| highgui | GUI API only; current window backend is NONE |
| highgui-native | Select GTK3 on Linux x86_64 glibc, Win32 on Windows, or Cocoa on macOS |
| highgui-gtk3, highgui-win32, highgui-cocoa | Explicit native backend; mismatched targets are refused |
| dnn | OpenCV's native DNN implementation; does not enable ORT |
| dnn-onnxruntime | Adds dnn and the official ORT 1.25.1 CPU SDK through mcpplibs.mwrap.onnxruntime |
| unifont | Unicode/CJK font rendering, implying imgproc |

The all-modules feature selects sixteen implementations; it does not select
external SDKs or every runtime backend. GTK3, Win32 and Cocoa window behavior
and ORT CPU inference have native evidence in the coverage/readiness records;
each new source checkpoint still requires its own platform CI. Physical camera
capture and GPU/provider execution require applicable device runners.
Calling imshow with API-only highgui still reports a missing window backend;
enable highgui-native or a supported explicit backend. Planned/unavailable codec,
GUI, video and accelerator names are listed in the coverage matrix and are not
registered as empty features.

## Modules, API and macros

```cpp
import mcpplibs.mwrap.opencv;
// Or import the selected module directly:
// import mcpplibs.mwrap.opencv.imgproc;
```

The root re-exports core, macros and the selected optional modules. Each public
submodule also re-exports core and its required module dependencies, with its
header adapter in a separate internal unit. The aggregate
selection module is generated into the configuration-specific build directory
before scanning, with direct imports. Optional modules are not exposed merely
because their source exists in the checkout.

`cv::Mat`, `cv::cvtColor` and other upstream APIs keep their spelling. Typed macro
mappings use a separate namespace:

```cpp
namespace macros = mcpplibs::mwrap::opencv::macros;
cv::Mat image(64, 64, macros::CV_8UC3, cv::Scalar(20, 80, 160));
```

Migration from the legacy wrapper uses these explicit replacements:

| Legacy form | New candidate form |
|---|---|
| `opencv.opencv@5.0.0` | `mcpplibs.mwrap.opencv@5.0.0.1`; core-only default, with other features requested explicitly |
| `import opencv;` / `import opencv.cv;` | `import mcpplibs.mwrap.opencv;` |
| `import opencv.<module>;` | `import mcpplibs.mwrap.opencv.<module>;` with its module feature enabled |
| Wrapper constant `cv::CV_8UC3` | `macros::CV_8UC3` with the namespace alias above |
| Ordinary upstream `cv::Mat`, `cv::cvtColor`, etc. | Keep the original spelling |

Historical package identities and resource bytes remain available unchanged;
the new package does not add permanent aliases for the legacy module names.

For preprocessing macros such as CV_Assert, include
`<mwrap/opencv/macros.hpp>` **before** importing. Raw macros still expand inside
qualified names: use raw macro and typed-mapping forms in separate contexts.


The typed mappings include all 13 OpenCV 5 scalar depths, channel families
(`CV_16BFC(n)`, `CV_BoolC(n)`, `CV_64UC(n)`, etc.), HAL value constants and
matrix type/layout helpers. Regenerate them with `python3 tools/generate_macros.py`;
`--check` verifies the committed result. Annotation and control-flow macros remain
in the textual side header. The typed `CV_IS_SUBMAT` mapping repairs OpenCV 5.0.0's
undefined `CV_MAT_SUBMAT_FLAG` reference by using the actual `CV_SUBMAT_FLAG`;
the optional raw side header applies the same repair while the vendor file
stays unchanged. Tests compare these helpers to real Mat
allocations and ROI flags.

For a facade-visible C++ inventory, first run `mcpp test --features all-modules
--toolchain llvm@23.1.3 --strict`, then `python3 tools/audit_api.py --verify-exports`.
The report separates declaration signatures, header ownership, internal linkage
and listed exports, and compiles a separate import-only name-lookup consumer.
Unloaded public headers and unlisted declarations remain review work; successful
name lookup alone does not prove overload or algorithm coverage. CI publishes
the inventory and consumer log for review.

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

The named `video` template encodes and decodes 12 generated frames through FFmpeg and lists native camera backends without requiring a camera. Linux V4L2 tests cover registration and the missing-device path; physical camera capture remains unverified.

```sh
mcpp new video-demo --template mcpplibs.mwrap.opencv@5.0.0.1:video
cd video-demo
mcpp run
```
