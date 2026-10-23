# Implementation and release readiness ledger

The full objective remains the OpenCV 5/main C++ API and backend plan, ORT wrapper, executable -m templates, one checkpoint PR per repository, green relevant CI and ecosystem review. Nothing is merged or released by these checkpoints. mcpp engine code stays read-only; its existing documentation-only PR is not a prerequisite.

## Verified current checkpoints

- OpenCV remote head 02fb836: native CI run 40026710108 passed all five jobs, including core, image codec behavior and the real default project template; Linux GCC additionally runs DNN/unifont. Root LICENSE/metadata/readme now consistently say Apache-2.0. The image example explicitly requests required features.
- ORT head de3a0a9: native CI run 40022727603 passed Linux GCC/LLVM, macOS arm64 and Windows MSVC ABI. Each runs three module consumers plus the actual project template. LLVM audits the exact SDK declaration inventory. Supplemental official headers omitted from Windows SDK are source/hash pinned. The config-module internal-linkage adaptation and mixing limits are explicit.
- SDK provider: isolated SDK install plus LLVM/GCC inference/header tests passed; no wrapper/source cache was preseeded in that SDK test.
- ORT immutable archive: source de3a0a9da143f4ae52eb5e1a0302ba3fdf623b6d downloaded twice with SHA256 24d4bc26d2f426e3643f21d289d92e7a23b3ba54524a099a1079c1c490915251. Index consumers execute real inference and public helper/config APIs. The complete unchanged publication layout also passed in a fresh MCPP_HOME: actual archive/SDK/toolchain installation, mcpp new, strict build and unmodified template run.
- ORT Linux distribution check: a release-mode consumer installed the actual immutable indexed archive and official SDK, then `mcpp pack --mode self-contained --format dir --toolchain llvm@23.1.3` succeeded. Its copied bundle passed Identity inference from `/` with a fresh HOME and MCPP_HOME/XLINGS_HOME/LD_LIBRARY_PATH/LD_PRELOAD absent. The bundled loader's `--list` resolved every file-backed ELF dependency inside the relocated bundle. This verifies Linux LLVM relocation, not Windows/macOS or GCC deployment. The initial license collector omitted ThirdPartyNotices.txt; index checkpoint 8d0f3cb now declares LICENSE and ThirdPartyNotices.txt deployment. A complete fresh-home archive/template/pack/relocation rerun passed and verified the bundled notices against the resolved SDK bytes. macOS deployment subsequently exposed a missing @loader_path rpath, corrected in index checkpoint 7d8b3ff and pending native CI revalidation.
- Index PR #564 contains the ORT wrapper descriptor, SDK provider, mwrap specification/skill and category/test discovery changes. The full updater audit passed (85 automatic declarations, one manual immutable snapshot), and all 14 updater adapter tests passed. Namespace-filtered views are test-only byte copies; normal publication layout is tested separately and is never converted.
- Llama PR #5 adds b10069.3 CPU template, keeping its legacy package/module identity. No-argument startup and a checksummed GGUF decode/sample passed through actual mcpp new consumers. Root LLVM build and all 116 existing unit/repository/build-helper checks passed. All eight checks on head 2de7714 passed, including Vulkan, Metal and Linux arm64. Its new archive/index descriptor and consumer were added in checkpoint 4018413; current native index validation remains required.

## Native window checkpoint (new implementation)

- Added highgui-native and explicit highgui-gtk3/highgui-win32/highgui-cocoa features, retaining API-only highgui and empty defaults. Target mismatches are errors. Current GTK SDK closure is Linux x86_64 glibc only; other Linux architectures and musl are refused.
- GTK resolves declared xim SDKs through a scoped pkg-config view. It does not fall back to host headers/libraries. Only the GTK implementation TU uses the pthread compiler mode, keeping public BMIs consumable without that flag.
- Bundled zlib uses upstream Z_PREFIX and Unix codec implementation symbols use hidden visibility, preventing their collision with shared GUI SDK dependencies.
- Linux LLVM and GCC strict tests passed all seven binaries, including actual native window creation, image display, event processing, geometry checks and destruction, plus PNG/JPEG roundtrips. The actual named window template also passed strict LLVM build and timed smoke execution.
- Cocoa passed its native CI job on head 2a74a82; Win32 validation is still running. These results must not be inferred from the earlier five green API/codec jobs. The template helper now restores package metadata reading and explicitly performs a strict build before running.

## Sixteen main-module checkpoint

- Added calib, features, objdetect, photo, ptcloud, stereo, stitching and video from the official pinned archive without changing upstream bytes. All sixteen modules are selectable and all-modules expands only their dependency closure; codecs/backends/SDKs stay explicit.
- Every retained upstream file (2,305 files) passed direct byte comparison against the SHA256-verified archive. An isolated actual importer rerun reproduced the exact retained file set. The importer now targets upstream/opencv and verifies caller-supplied source trees, building a complete replacement before replacing an existing import.
- Added behavior tests for SIFT, QR encoding, stereo block matching, camera intrinsic estimation from projected views, denoising, four-channel depth-to-point-cloud conversion, stitching's insufficient-input status, Kalman prediction and optical flow. Both direct submodule imports and the root aggregation are exercised.
- Linux LLVM 23.1.3 and GCC 16.1.0 strict tests passed eight binaries, including all eight new module behavior cases. Added this same all-modules test gate to every native CI leg, including musl.
- SIFT and pyramidal optical-flow dispatch currently use upstream baseline implementations; FAST additionally uses its x86 AVX2 TU. New-module OpenCL includes preserve upstream's macro definitions, but explicitly reject HAVE_OPENCL until real kernel integration exists.
- Existing header-parser/curated exports still require a complete public C++ AST inventory and documented omissions review. Main-module build coverage does not prove complete public API coverage or hardware backend coverage.

## ORT bridge and current-client checkpoint

- The latest published stable mcpp changed during implementation: v2026.10.24.1 was published at 2026-10-23T22:02:51Z. Its Linux package checksum was verified. OpenCV, ORT, llama and index CI/tool pins now use 2026.10.24.1; no mcpp engine source was edited.
- ORT head 2c3495f passed all four native jobs on the new client. Its source archive was downloaded twice with SHA256 26a89e349d3eb01e339d3c091ff0371ccc4401026f94698bef8e607ad27bc407. The index descriptor now points at that exact commit.
- Llama head f5a046c passed all eight native checks on the new client. Local strict package build, all 116 unit/repository checks, and actual rendered GGUF decoding passed (sampled token 471). Its archive was downloaded twice with SHA256 543c19018aa05a7e6813f692a3535b6c5953842fdd70041e63266b5864435ddd. Index adds b10069.3 without changing any historical entries or the legacy ggml-org.llamacpp/import llamacpp identity.
- The index's new llama CPU consumer executes real APIs from the archived candidate; a fresh complete-index template consumer also decoded the pinned GGUF. Its nonnumeric upstream release key is preserved. mcpp --strict treats the supported exact-key warning as an error, so that explicit legacy test exception accepts only that diagnostic and rejects other build warnings. Numeric ORT/OpenCV consumers retain --strict.
- Added dnn-onnxruntime, implying dnn and the standalone ORT wrapper/official CPU SDK. Unsupported target/ABI combinations fail during configuration. The all-modules aggregate does not imply this external SDK feature.
- A generated ONNX Add model runs through ENGINE_ORT. Tests require a real ORT JSON profile containing both model_run and CPUExecutionProvider; OpenCV's native-engine fallback cannot pass. LLVM and GCC strict tests passed all nine binaries on mcpp 2026.10.24.1. The actual named onnxruntime template also passed GCC and LLVM build/run; it imports both wrappers and checks output values.
- Cross-package GCC testing exposed a default-versus-explicit BMI mapper root mismatch for the library's own aggregate. Re-exporting ORT from the feature-selected aggregate records the complete named-module dependency graph and fixed the test without changing mcpp, removing the interop test, or requiring private imports in user code.
- Windows ORT profiling needs ORTCHAR_T/wchar_t rather than OpenCV's char string. A private generated implementation TU adapts that call through port/include/mwrap/opencv/ort_profile_path.hpp; the upstream file remains byte-identical. Native Windows validation remains required.
- Full-module CI on f6b1e5e passed macOS, Linux LLVM and musl, but Windows found M_PI absent in tracker_vit.cpp. Added upstream's _USE_MATH_DEFINES and standard constant defines to the eight new modules' private flags. The next native matrix must validate this correction, Win32 windows and the ORT bridge.
- Index checkpoint 4018413 adds archive/template checks for llama, notice deployment requirements in the shared specification/skill, new artifact pins and latest-client CI. All 14 updater adapter tests and the complete updater audit passed; every historical version/resource was preserved. Native CI for this new index head is pending.

## Review decision

| Repository | Merge readiness against the requested scope | Release readiness | Index readiness |
|---|---|---|---|
| opencv-m | Not ready: complete local main-module baseline, but API/backend scope and new native CI pending | Not ready for a full-support release | Final source archive descriptor and installed consumers absent |
| onnxruntime-m | CPU-only candidate has four green native jobs; final API/distribution review pending | No release/tag created; Linux notices/relocation passed, platform deployment verification pending | SDK/wrapper descriptors and Linux real archive/template consumption implemented in index PR; final cross-platform validation/review pending |

The public repository listing proves repository creation/forking, not merge or release readiness. Both source PRs and the index PR remain drafts. The latest index validation/platform workspace jobs passed, but its openkal-compat measurement job was still running at this audit; the whole PR cannot be described as green yet. ORT CPU acceptance is independent of OpenCV full support.

## Live work and remaining gates

- Monitor current index and llama PR heads, not the earlier failing runs. Index Windows UTF-8 decoding and current shared-image adapter metadata were corrected. Llama Vulkan CI pins the candidate index so it can validate those fixes before merge; native ARM verifies managed ELF runtime closure rather than assuming a static file label. No strict checks were disabled.
- All 16 main modules are imported; all-modules strict tests pass on Linux LLVM/GCC. New native matrix results and full public API coverage remain required.
- Finish native highgui Win32 validation, video native backends and Windows/macOS ONNX Runtime bridge validation with concrete features/templates/actual runtime tests. The highgui feature alone remains API-only; native backends require explicit selection. The new dedicated ORT test proves CPU execution through the session profile; the ordinary dnn test alone does not.
- Complete public OpenCV API/macros inventory and consumer/mixing audit, platform/backend/codec matrices, and explicit unsupported hardware status remain required.
- Add the finalized OpenCV archive descriptor and consumers to pkgs/mwrap/tests/mwrap only after source/features and archive bytes are validated.
- Verify packaging/deployment/relocation and distribution runtime closure, including external SDKs, rather than relying solely on running inside an installed mcpp home.
- Validate latest index archive/template checks on every declared platform and inspect all PR checks to green. Update stale architecture/contribution docs and re-review all source/feature/version/license/runtime boundaries.
- Audit all local -m packages for runnable templates. ffmpeg-m/grpc-m/imgui-m already have templates; OpenCV and ORT are tested, llama fills the discovered missing template. Do not rewrite legacy published package identities as a shortcut.
- Final PR descriptions, release ordering and requirement-by-requirement readiness audit remain mandatory. No merge, tag or release until user review.

## PRs

- https://github.com/mcpplibs/opencv-m/pull/1
- https://github.com/mcpplibs/onnxruntime-m/pull/1
- https://github.com/mcpplibs/mcpp-index/pull/564
- https://github.com/mcpplibs/llama.cpp-m/pull/5

## Native video and cross-platform text checkpoint

- Added videoio-native with explicit videoio-msmf/videoio-avfoundation overrides. Linux uses the existing V4L2 catalog; Windows MSVC ABI uses Media Foundation; macOS uses the upstream macOS AVFoundation implementation. Target/backend mismatches fail during configuration.
- Linux LLVM 23.1.3 strict testing with videoio-native,videoio-ffmpeg passed ten binaries. V4L2 registration/missing-device behavior passed, and FFmpeg encoded and decoded twelve generated frames with backend identity, dimensions and image error checks. This is not physical camera coverage.
- Every native CI leg now runs this feature combination. Windows/macOS additionally exercise native H.264 MP4 encode/decode; those new checks remain pending. The named video template performs an FFmpeg roundtrip without camera hardware; its actual scaffold/build/run is being validated locally and in CI.
- On c59b226, macOS, Linux LLVM and musl passed. Windows passed all sixteen module tests and the real Win32 window, then failed a generated ONNX header byte comparison due to checkout line ending conversion. Added LF attributes for wrapper text and disabled conversion for the pinned upstream tree/binary assets; the generated-model check passes locally. This does not yet prove Windows ORT execution.
- Index f0f8e48 updates current llama CPU/Metal examples to b10069.3, which carries the portable LLVM 23 include fix. Historical descriptor versions remain unchanged. The replacement native runs are live; the prior b10069.2 failure is not waived.

## OpenCV 5 typed macro checkpoint

- Replaced the hand-maintained eight-depth subset with generated mappings from the pinned core HAL header: all thirteen scalar depths, all channel families/constants, HAL numeric constants and pure matrix type/layout helpers. Declaration and control-flow macros remain textual.
- Added actual Mat allocation/layout checks for every depth and channels 1,2,3,4,7,128; verified unsigned/signed 64-bit, unsigned 32-bit and bool storage access; checked ROI/continuity against Mat methods. LLVM 23.1.3 and GCC 16.1.0 strict tests each passed eleven binaries, including all three new layout behavior cases. These tests do not claim every arithmetic algorithm supports every new depth.
- OpenCV 5.0.0 CV_IS_SUBMAT uses an undefined CV_MAT_SUBMAT_FLAG. The typed mapping explicitly uses CV_SUBMAT_FLAG, tested against Mat::isSubmatrix; upstream source and the raw side-header macro remain unchanged. Both readmes document this adaptation.
- CI checks generation consistency on every platform. Native macro validation remains pending; the prior Linux video template rendered, strictly built and decoded twelve frames successfully on the latest client.

## C++ inventory and core surface restoration checkpoint

- Added a streaming Clang AST inventory of facade-visible namespace/global types, aliases, enums/enumerators and overload signatures, with header ownership and internal-linkage flags. Class members ride with their owning class. ABI-versioned dnn namespace imports normalize to public cv::dnn names. Unloaded headers are reported separately; this is not a full-public-header completeness claim.
- Corrected Clang JSON's stream-wide file-name elision, including locations in otherwise skipped function/class bodies. Three focused parser regressions cover streaming/truncation, cross-header ownership and namespace/overload/enum/linkage behavior. A real Clang nested-header experiment reproduced the ownership failure before the correction.
- Restored public core Affine3/Quat/AutoBuffer and related types, fixed-width/global scalar aliases, 64-bit Vec aliases and the complete CPU enum surface without moving upstream declarations into new namespaces. Regeneration now supports explicit public global declarations. Affine's TU-local free operators use explicit adapters in the existing replacement namespace; upstream files remain unchanged.
- New module-only tests exercise affine composition/inversion and Point3/Vec transforms, quaternion rotation, growing AutoBuffer, partition grouping, global aliases and new scalar types. LLVM 23.1.3 and GCC 16.1.0 strict core suites each passed twelve binaries. The LLVM sixteen-module strict suite also passed twelve binaries, including all eight main-module algorithm cases.
- The corrected audit inventories 3,323 facade-visible namespace/global entities and reports 624 not present in export-using lists. Some have manual adapters, while others are real gaps; they are not silently pruned or declared complete. A separate root-import consumer successfully resolved all 2,700 listed export names. This validates name reachability, not every overload/template instantiation. CI now runs the inventory and publishes its evidence.
- Latest index f0f8e48 native Windows/macOS workspaces and all four llama candidate checks passed; Linux workspace/measurement checks remain live. No source PR or index PR has been merged.
