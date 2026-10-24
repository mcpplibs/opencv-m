# OpenCV 5.0.0 coverage

These matrices describe verified scope, not a complete-support release. The
[readiness ledger](../.agents/docs/2026-10-24-readiness-audit.md) records exact
commits, CI runs and remaining acceptance gates. Vendor bytes are pinned in
upstream/sources.lock.toml; native CI for e72c09b passed all five jobs. Later
public-API restorations are locally verified and require their own native CI.

## Source modules

All interfaces use `mcpplibs.mwrap.opencv.<module>` and preserve upstream API
namespaces. Core is always present. `all-modules` selects the other fifteen
features and their required dependency closure. Every module also depends on core.

| Module / feature | Other required modules | Source and import coverage |
|---|---|---|
| `calib` | `flann`, `geometry`, `imgproc`, `objdetect`, `stereo` | retained / builds / direct import / aggregate |
| `core` | mandatory | retained / builds / direct import / aggregate |
| `dnn` | `geometry`, `imgproc` | retained / builds / direct import / aggregate |
| `features` | `geometry`, `imgproc` | retained / builds / direct import / aggregate |
| `flann` | core only | retained / builds / direct import / aggregate |
| `geometry` | `flann` | retained / builds / direct import / aggregate |
| `highgui` | `imgproc` | retained / builds / direct import / aggregate |
| `imgcodecs` | `imgproc` | retained / builds / direct import / aggregate |
| `imgproc` | `geometry` | retained / builds / direct import / aggregate |
| `objdetect` | `features`, `geometry`, `imgproc` | retained / builds / direct import / aggregate |
| `photo` | `geometry`, `imgproc` | retained / builds / direct import / aggregate |
| `ptcloud` | `flann`, `geometry`, `imgproc` | retained / builds / direct import / aggregate |
| `stereo` | `geometry`, `imgproc` | retained / builds / direct import / aggregate |
| `stitching` | `features`, `flann`, `geometry`, `imgproc` | retained / builds / direct import / aggregate |
| `video` | `imgproc` | retained / builds / direct import / aggregate |
| `videoio` | `imgcodecs`, `imgproc` | retained / builds / direct import / aggregate |

## C++ API and macros

| Surface | Evidence | Remaining boundary |
|---|---|---|
| Root and sixteen direct modules | 3,217 export-list names compile in the latest local all-module Clang audit | Name lookup does not prove every overload/template/backend |
| Public core types, filesystem, TLS, parallel backend, dual quaternions, async promise | Actual behavior tests; LLVM/GCC strict suites | Full core.hpp mixing on GCC has a declaration-merge limitation |
| Scalar conversion adapters | Independent pinned SDK oracle, 169 type pairs; saturation/rounding boundaries | Platform rounding bridge needs current native matrix |
| Generic min/max and makePtr | Reference/tie and image behavior; positive/negative custom-deleter compilation | GCC shared_ptr probes include the standard memory header |
| FLANN and stitching detail | Direct-import algorithm and factory consumers | Other portable helper headers still require inventory review |
| Macro constants and pure calculations | Generated `.macros` mapping and optional textual macro header; ROI behavior | Preprocessor flow controls remain textual; no fake exported preprocessor state |
| DNN-dependent DISK | Conditional export and missing-model rejection; features-only build without DNN | Trained DISK inference is not verified |
| Remaining API/header inventory | 3,395 facade-visible entities; 179 not in export lists; 119 not-in-facade headers | Includes manual adapters, private/legacy/device headers, plus real portable helper gaps; each needs classification |
| DNN shape helpers | Twenty generated overloads retain pinned bodies; direct-import tensor/axis/formatting behavior passes LLVM/GCC | Static-inline bodies use a constrained named-module adapter |
| Persistence helpers | 65 pinned SDK bodies plus typed forwards; native header/module XML/YAML/JSON and Base64 bytes match on GCC/LLVM | Private vector proxies stay internal; original FileStorage/WriteStructContext types retain their SDK identity |
| Geometry optimizer extension | Public LevMarqBase/Backend imported; callback rejection/geodesic report contracts execute original SDK code | Device acceleration is separate |

## Runtime capabilities

| Feature / capability | Verified behavior | Conditions / limits |
|---|---|---|
| `highgui` | API can be imported | No window backend by itself |
| `highgui-native` | GTK3 / Win32 / Cocoa create, display, process events and destroy | GTK SDK is Linux x86_64 glibc only; Linux uses Xvfb in CI |
| `videoio-native` | V4L2 registration/error path; MSMF/AVFoundation file tests | No physical-camera claim |
| `videoio-ffmpeg` | Generated file encode/decode and actual video template | Declared FFmpeg SDK; managed NASM on x86_64 Linux/Windows |
| Base image codecs | BMP/PPM exact roundtrips; PFM float roundtrip | Other compiled base formats are not individually certified |
| `imgcodecs-png`, `imgcodecs-jpeg` | Generated image roundtrips | Separate explicit features |
| `dnn` | Generated ONNX Add through native classic/new engines | No external accelerator implied |
| `dnn-onnxruntime` | Generated ONNX Add; actual CPUExecutionProvider session profile and template | Official ORT 1.25.1 CPU SDK, supported targets only |
| CUDA, OpenCL, OpenVINO, other SDK backends | Planned/unavailable | Declaration exports do not enable a device implementation |
| Contrib | Not imported | Separate matching upstream version and per-module verification required |

## Targets and configuration

| Target | Toolchain / linkage | Verified scope at e72c09b |
|---|---|---|
| Linux x86_64 glibc | GCC 16.1.0; LLVM 22.1.8 CI / 23.1.3 local; archive implementation | All modules, codecs, native video/FFmpeg; GTK window on LLVM; ORT CPU |
| Linux x86_64 musl | GCC 16.1.0; static profile | All modules, codecs, native video/FFmpeg; declared GTK/ORT SDKs unavailable |
| macOS arm64 | LLVM 23.1.3; native frameworks | All modules, codecs, Cocoa, AVFoundation/FFmpeg, ORT CPU |
| Windows x86_64 MSVC ABI | LLVM 23.1.3 | All modules, codecs, Win32, MSMF/FFmpeg, ORT CPU |
| Other triples / ABI / shared-library configurations | Not certified | No inference from the target rows above |

The Linux x86_64 glibc source project and the ORT template explicitly select
`cxx_runtime = "toolchain-coupled"`. A verified negative test showed that embedding
a static exception runtime while loading the SDK could terminate typed catches;
the shared-runtime configuration passed the same tests.

Installed immutable OpenCV archives, relocated deployment/runtime closure and
all final source/index CI heads remain release gates. Main-module CI does not
establish those distribution properties. No tag, release or merge is created.
