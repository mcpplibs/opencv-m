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
| Root and sixteen direct modules | 3,457 export-list names compile in the latest local all-module Clang audit | Name lookup does not prove every overload/template/backend |
| Public core types, filesystem, TLS, parallel backend, dual quaternions, async promise | Actual behavior tests; LLVM/GCC strict suites | Full core.hpp mixing on GCC has a declaration-merge limitation |
| Scalar conversion adapters | Independent pinned SDK oracle, 169 type pairs; saturation/rounding boundaries | Platform rounding bridge needs current native matrix |
| Generic min/max and makePtr | Reference/tie and image behavior; positive/negative custom-deleter compilation | GCC shared_ptr probes include the standard memory header |
| FLANN and stitching detail | Direct-import algorithm and factory consumers | Other portable helper headers still require inventory review |
| Macro constants and pure calculations | Generated `.macros` mapping and optional textual macro header; ROI behavior | Preprocessor flow controls remain textual; no fake exported preprocessor state |
| DNN-dependent DISK | Conditional export and missing-model rejection; features-only build without DNN | Trained DISK inference is not verified |
| Remaining API/header inventory | 3,948 facade-visible entities with SIMD selected; 255 not in export lists; 102 not-in-facade headers | Includes manual adapters, private/legacy/device headers, plus real portable helper gaps; each needs classification |
| DNN shape helpers | Twenty generated overloads retain pinned bodies; direct-import tensor/axis/formatting behavior passes LLVM/GCC | Static-inline bodies use a constrained named-module adapter |
| Persistence helpers | 65 pinned SDK bodies plus typed forwards; native header/module XML/YAML/JSON and Base64 bytes match on GCC/LLVM | Private vector proxies stay internal; original FileStorage/WriteStructContext types retain their SDK identity |
| Geometry optimizer extension | Public LevMarqBase/Backend imported; callback rejection/geodesic report contracts execute original SDK code | Device acceleration is separate |
| Value operators and portable helpers | 122 pinned types.hpp operator bodies, fifteen core helper bodies and image conveniences; independent SDK/module results match on LLVM/GCC | rawInOut(std::array) uses the original non-static SDK member through an instance to repair the broken free-wrapper call |
| Matx/Vec template signatures | Original template order, every explicit argument prefix in eight adapted families, function pointers and derived-to-base deduction; independent 342-byte SDK/module oracle passes GCC16/LLVM22/LLVM23 | Separate imported-type producer avoids the Clang22 GMF serialization crash; current all-module/native matrix still required |
| Preview plugin ABI and point-cloud rendering | Original CvResult/OpenCV_API_Header entities and two CPU rendering functions; three-compiler SDK/module and rendering checks pass | Preview ABI follows upstream compatibility limits; declaration availability does not enable a plugin backend |
| CPU-usable CUDA helpers and feature HAL | Eight helper overloads plus ten SDK Event flag operators; CPU Mat reuse/reshape and 424-byte SDK/module oracle pass on GCC16/LLVM22/LLVM23; original cvhalKeyPoint is exported | No CUDA device backend or GPU allocation is certified; 26 extra HAL macro values and typed FOURCC mapping are tested separately |
| Public core diagnostics and legacy enums | Twelve SDK dump forwards, eight original external utility declarations, and original photo/video global enum spellings; three-compiler SDK/module oracle matches 1302 bytes | Ignored language-binding tests remain outside the exported public surface; cache accessor signature is checked without changing user cache state |
| Qualified FP controls and videoio opaque types | Six original cv::details state/functions/RAII declarations, plus IVideoCapture/IVideoWriter forwards; independent SDK/module state restoration and exact signature checks pass GCC16/LLVM22/LLVM23 | Per-thread state is restored; unsupported architectures retain SDK no-op behavior. Opaque declarations do not provide backend implementations |
| Global fast math | Twenty exact SDK forwards cover cvRound/cvFloor/cvCeil, cvIsInf/cvIsNaN and CV_FOURCC; SDK/module output matches on GCC16 and LLVM22/23, including mixed-header checks | Platform rounding follows the SDK; native CI on the current checkpoint remains required |
| Public scalar/constant aliases | Original cvTrunc/cvRound64 and utils::getThreadID; ArUco threshold matches the SDK detector and supports address access | Naming the ArUco threshold after both its original header and module import is ambiguous; use module-only access for that value |
| Core/imgproc HAL | 172 core and 34 imgproc public names; saturating arithmetic/norms/exp and color conversions execute the SDK | SDK IGNORED legacy aliases and private filter classes stay outside this surface; ISA intrinsics remain separate |
| Instrumentation external interface | Original NodeData/InstrNode types and SDK state functions; both setFlags overloads and NodeData equality | Instrumentation collection is disabled in the current build; tests verify original no-op behavior |
| Tracking extension interfaces | Ten documented SDK classes/aliases; a user feature runs through TrackerFeatureSet, plus target-state and sampling behavior | Pinned sampler returns false while producing samples; the wrapper preserves this upstream behavior |

## Runtime capabilities

| Feature / capability | Verified behavior | Conditions / limits |
|---|---|---|
| `highgui` | API can be imported | No window backend by itself |
| `highgui-native` | GTK3 / Win32 / Cocoa create, display, process events and destroy | GTK SDK is Linux x86_64 glibc only; Linux uses Xvfb in CI |
| `videoio-native` | V4L2 registration/error path; MSMF/AVFoundation file tests | No physical-camera claim |
| `videoio-ffmpeg` | Generated file encode/decode and actual video template | Declared FFmpeg SDK; managed NASM on x86_64 Linux/Windows |
| Base image codecs | BMP/PPM exact roundtrips; PFM float roundtrip | Other compiled base formats are not individually certified |
| `imgcodecs-png`, `imgcodecs-jpeg` | Generated image roundtrips | Separate explicit features |
| `imgcodecs-webp` | Lossless transparent RGB/alpha, animation frames/timing/loop count and exact XMP bytes | libwebp 1.5.0 with mux/demux; Linux GCC16, LLVM22/23 local checks pass; native/archive gates pending |
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

## Reviewed header decisions

`port/api/header-review.toml` records every header from the initial 113-header
facade gap list with an explicit reason. It distinguishes compatibility
forwarders, SDK/platform code, upstream internals, textual macros and actual
public gaps. HAL record/value, public core dump utilities and legacy enum coverage have since been implemented.
`tools/check_header_review.py` checks review coverage against the compiler
inventory; `--require-complete` additionally rejects the 20 remaining public-gap
records (universal intrinsics and their architecture implementations). Review coverage does not mean these APIs are implemented.

## Native SIMD checkpoint

The opt-in `simd` feature exposes `mcpplibs.mwrap.opencv.core.simd`, plus the
selected root import, with 220 enabled baseline names on Linux x86_64. Original
SDK vector types are retained. GCC uses producer-compiled memory forwards to
preserve unaligned input/output semantics; Clang re-exports SDK inline entities.
The 495-byte native SDK/module oracle passes GCC16, LLVM22 and LLVM23, alongside
all 28 programs with `all-modules,simd`, 35 baseline interface checks and makePtr
positive/negative contracts. GCC release-mode parity and export visibility pass.
See [SIMD configuration](simd.md). Native ARM64/Windows CI remains required.

Header review keeps eight native-family records and two software/legacy records
partially accepted, with ten other mode/ISA records unimplemented.
Full-support acceptance fails for all unfinished categories. The opt-in
`simd-emulated` mode preserves generic SDK vector templates and has independent
software SDK parity/lookup checks. The explicitly imported `core.simd.legacy`
re-exports the 21 original operators; both vector modes have SDK parity and
base/root visibility-boundary checks. Non-baseline ISA families remain uncertified.
