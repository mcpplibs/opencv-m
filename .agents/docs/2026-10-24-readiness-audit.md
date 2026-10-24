# Implementation and release readiness ledger

The full objective remains the OpenCV 5/main C++ API and backend plan, ORT wrapper, executable -m templates, one checkpoint PR per repository, green relevant CI and ecosystem review. Nothing is merged or released by these checkpoints. mcpp engine code stays read-only; its existing documentation-only PR is not a prerequisite.

## Latest review state

The six companion source PR heads were rechecked through GitHub: ORT 576c780b
(4 checks), llama.cpp 5028a1aa (8), FFmpeg 1e8bce6e (2), gRPC 0d11b223 (6),
ImGui ffebf83b (4) and Godot 40f72d55 (3) all have successful checks. Their
independent draft PRs remain open, with executable templates included.

OpenCV's 16baa03 native matrix passed all five jobs. Native run 40088385850
on d1c3152 is running; the earlier eb1d34b Windows leg passed all-main-module
tests, including the formerly crashing ArUCo constant case, before cancellation.
The software SIMD checkpoint now passes all 28 programs on all three Linux
compilers. Current Matx producer separation restores SDK template argument forms
and passes independent three-compiler parity; its new 30-program combination
and native matrix remain required. Index ee52fdb corrects the mixed-workspace
ambiguity that persisted on 6ff350a; its site check passes, while workspace
validation, updater and OpenKAL checks remain pending.
The index still pins the independently verified 019eaadb OpenCV archive.
Final source/archive alignment and all relevant exact-head checks remain
required. API/header and backend classification is unfinished. No merge/release
readiness is claimed.

Sections below are chronological checkpoint evidence, not current PR status.

## Historical verified checkpoints

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

## Public module closure and advanced stitching checkpoint

- Actual direct stitching import exposed missing core name visibility. GMF reachability alone did not expose cv::Mat/Point/Rect names. Public optional modules now aggregate core, required modules and independent internal header-adapter modules under mcpplibs.mwrap.opencv.detail; C++ namespace/type identities and public import names remain unchanged.
- Directly combining header GMFs and import core in one unit hit GCC 16's SVD duplicate defaults. Standard interface partitions subsequently hit a GCC merge ICE. Independent named adapter units passed; no compiler/engine patch or upstream-header edit was made. The source generator preserves adapter bodies and regenerates the public dependency aggregations and manifest source lists.
- Restored 99 public stitching/detail exports and included the previously unloaded autocalib/timelapsers headers. Tests exercise actual two-tile CPU blending, camera intrinsics/plane projection and timelapser placement through the direct stitching import. Both LLVM 23.1.3 and GCC 16.1.0 strict all-modules suites passed thirteen binaries, including these three cases and all eight main-module behavior cases.
- Added actual native ONNX Add execution to both ENGINE_CLASSIC and ENGINE_NEW, independent of ORT; both passed in the LLVM/GCC all-module suites. Added pixel-exact BMP/PPM and float-preserving PFM roundtrips, also passing both suites. PNG/JPEG remain separately gated codecs.
- The updated Clang audit inventories 3,328 facade-visible namespace/global entities, 530 not in export-using lists; 132 of 274 retained header candidates remain outside the facade configuration and 14 are private. The separate consumers resolved all 2,799 listed root export names and all sixteen direct module surfaces, checking their own exports plus core type visibility. Template signatures now include the underlying function/alias declaration. Missing names/header profiles still require review; no completeness claim is inferred from these counts.
- Replaced the text search over Ninja rules with a resolved compile-command guard. All 33 public/internal wrapper module interfaces passed the baseline ISA check. CI runs it on every platform and after all-modules, and publishes direct-import audit evidence.
- Refreshed both architecture documents to describe the actual sixteen-module/provider/feature/adapter layout and remaining API/mixing boundaries.

## Existing -m template ecosystem checkpoint

- Found and corrected duplicated namespace rendering in ffmpeg-m, grpc-m and imgui-m templates: current self.name already contains the full qualified identity. Each repository has its own draft PR/checkpoint; published legacy package/module identities remain unchanged.
- FFmpeg PR #8 / 5349a37 adds revision 8.1.2.1. Actual mcpp new, strict LLVM build and generated three-frame decoding passed locally; both native library and actual-template CI passed. Its full-commit archive was downloaded twice with matching SHA256 0c528d674aa99bf830fc61455cffdbf903e4c522c6da966b08274821acaa2d17. Installed-archive/index validation remains pending.
- gRPC PR #8 / 56e6a2c adds revision 1.83.0.1 with runtime/plugin/rule version alignment, same-artifact plugin/rule path dependencies and current build-dependency declarations. Explicit and default greeter selectors both strictly built and executed successful/error RPC calls locally. Native/template CI is live on the latest checkpoint.
- ImGui PR #13 / f3d9d16 adds revision 1.92.8.1. The prior 2dc0001 checkpoint passed all three native library jobs plus actual window/docking scaffolds rendering three frames each. The new default headless template strictly built and generated 168 draw vertices locally with DISPLAY/WAYLAND_DISPLAY unset; desktop templates stay explicit. New native/template CI is live; its three library jobs passed at this audit.
- Local GUI validation initially exposed an incomplete warm graphics SDK cache: libllvm/libxshmfence payloads were absent and Mesa view links were stale. Official package-tool installation/activation of the declared SDKs restored normal mcpp strict build/run and three-frame rendering under Xvfb. A temporary isolated library-path experiment was diagnostic only, not acceptance proof; native CI independently passed without that override.
- Index f0f8e48 passed every applicable check, including all platform workspaces, four llama candidates, graphics isolation and the 40m51s OpenKAL measurement. This is the checkpoint before adding finalized additional wrapper archives/consumers, so it is not the final index acceptance.
- OpenCV 79a8b60 native CI passed macOS (including AVFoundation/FFmpeg file roundtrips, video template and ORT bridge/template) and musl (V4L2/FFmpeg). Windows and both glibc Linux jobs remain live. These results precede the new internal-adapter/direct-import checkpoint and do not validate that later source change.

## User resource and README constraints

- All -m root READMEs use the same concise entry structure and normally stay within 100 lines per language. Detailed usage moves to docs/; prerequisites, candidate status and critical scope limits remain visible. The index's bilingual mwrap spec and package-authoring skill record the rule.
- Local tests and subprocesses collectively stay below 80% of effective CPU/memory. This host currently has 32 logical CPUs and about 62 GiB RAM. The task-specific mwrap-tests.slice has CPUQuota=400%, MemoryHigh=28G, MemoryMax=32G and TasksMax=512; future local heavy checks use that shared slice with the stricter four-core quota, MCPP_JOBS=1 and one heavy task at a time. These limits apply only to this task's processes, with unrelated user jobs untouched. Check current load/available memory before further heavy work.

The current kernel cgroup limits are cpu.max=400000/100000, memory.high=28 GiB and memory.max=32 GiB. This user hierarchy does not delegate cpuset, so test commands additionally use taskset -c 0-3. Local heavy checks run as:

```sh
systemd-run --user --scope --slice=mwrap-tests.slice --quiet \
  taskset -c 0-3 env MCPP_JOBS=1 <check-command>
```

README rewrites cover OpenCV (EN/ZH), ORT, llama.cpp, FFmpeg, gRPC (EN/ZH) and ImGui. Each entry is below 100 lines, with its detailed prior content retained in docs/usage*.md and local links checked.

## FLANN and strict-consumer portability checkpoint

- Restored the public cv::flann aliases/GenericIndex and 164 non-internal cvflann names without changing their namespaces. Two direct-import runtime tests perform nearest-neighbor searches through the high-level and low-level APIs and check distance functions. LLVM 23/GCC 16 strict FLANN suites passed; the LLVM all-module suite passed fourteen binaries. The combined GCC all-module strict recheck also passed fourteen binaries in 250 seconds (247 seconds build), including both FLANN cases, stitching, both native DNN engines and codec roundtrips.
- The all-module Clang audit now inventories 3,328 facade-visible entities and resolves 2,983 listed exports through the root and all sixteen direct modules. It leaves 346 unlisted entities and 132 not-in-facade headers for review; these counts include internal declarations/manual adapters and cannot be called complete API coverage. Namespace const internal linkage now accounts for desugared types, pointer/reference constness and preceding external declarations; four parser regressions pass.
- Repaired the optional raw CV_IS_SUBMAT compatibility macro to match the existing typed mapping and Mat::isSubmatrix, while preserving vendor bytes. Actual ROI checks passed on GCC/LLVM.
- The previous remote GCC job failed during ORT tests because its BMI mapper mixed indirect adapter paths. Explicit aggregate imports of selected internal adapters fixed compilation. A retained candidate then passed fourteen strict binaries, including actual ONNX Add through ORT with CPUExecutionProvider profile evidence. Its first link attempt ran out of disk; the successful retry had sufficient free space and a 6 GiB cutoff guard.
- On remote edadf5c, LLVM, musl and macOS passed. Windows passed all modules, actual Win32 GUI and both MSMF/FFmpeg twelve-frame file roundtrips, then failed the video template's strict build because Strawberry Perl's host NASM was selected. The wrapper now declares managed xim:nasm@3.02 for x86_64 Linux/Windows videoio-ffmpeg; ffmpeg-m declares the same tool for its x86 profile. This is a library dependency declaration, with no engine/PATH/strict-mode workaround. Native Windows verification is pending.
- Latest README heads for ORT 576c780, llama.cpp 5028a1a, FFmpeg fa850f4 and ImGui ffebf83 passed all source checks. gRPC 839b61e has live native/template jobs. No PR has been merged or tagged.

## Portable public C++ exports checkpoint

Restored public error-code enums, Param/ParamType and Node, traits, sample helpers, logging, quaternion free functions, calibration fisheye flags, callback typedefs and geometry/point-cloud enums. The features surface now also exposes its C++ aliases and FlannBasedMatcher. DNN now exposes ActivationFunc, MatType, SoftNMSMethod and the documented dispatched activation lookup; removed the obsolete getActivationFunc prune entry after confirming its public all_layers.hpp declaration. Core explicitly loads logger.hpp instead of relying on another facade's transitive includes. Vendor source bytes and upstream namespaces remain unchanged.

LLVM 23.1.3 and GCC 16.1.0 strict all-module suites each passed fourteen binaries (106.62 and 197.77 seconds). New runtime cases exercise throwing/catching upstream error codes, traits/ParamType, logging level restoration, free quaternion inverse/power and dispatched ReLU. All 3,119 listed exports resolve through the root and every direct module in the exact Clang all-module configuration. The inventory remains 3,328 entities, with 210 not in export lists. There are still 132 not-in-facade headers; many are legacy umbrellas, SIMD/device implementations or optional SDK interfaces, but portable public headers such as dualquaternion.hpp, async_promise.hpp, filesystem.hpp, parallel_backend.hpp and geometry/mst.hpp require further restoration and verification. No complete-API claim is made.

A direct organization API audit found seven -m repositories, including godot-cpp-m previously omitted from the six-repository local inventory. Its latest main b19f824 targets godot-cpp 10.0.0-rc1 (Godot 4.6). It is now included in the template/concise-README work with its existing godotengine.godot-cpp-m and import godot_cpp identities preserved. The candidate wrapper revision is 10.0.0-rc1.1; local template validation is in progress, and it needs its own PR/index archive consumer.

## Portable header and scalar semantic checkpoint

The previous remote head e72c09b passed all five native jobs in run 40050010529: Linux GCC/LLVM/musl, macOS and Windows. In particular, Windows completed the actual ORT CPU profile/template plus Win32 and MSMF/FFmpeg/video-template checks. These results apply to that head; the following new changes need their own native matrix.

Restored portable dual-quaternion, async-promise, filesystem, TLS, FP-control and caller-provided parallel backend headers, geometry MST, and CPU-build CUDA/OpenGL declaration surfaces. GPU declarations do not activate device backends. New behavior tests exercise translation, async Mat transfer, temporary filesystem operations, thread-local isolation, parallel_for_ dispatch through a supplied backend, both Prim/Kruskal MST and CPU-only CUDA detection. DISK is exported only with DNN, with an invalid-model test; trained-model inference is not claimed.

An independent SDK oracle exposed three actual defects in the previous partial saturate_cast adapter: negative-to-unsigned wrap instead of saturation, ushort-to-uchar truncation, and double-to-uint64 truncation instead of rounding. The new generator preserves all thirteen pinned SDK source overloads and specializations; two ordinary private functions bridge TU-local cvRound. The test captures native function pointers before import and checks all 169 source/destination pairs, including rounding and boundary values. Generic cv::min/max now retain the actual standard-library entities, including first-reference ties, strings, comparators and initializer lists; dedicated Mat/UMat three-argument forwards preserve image overload selection. makePtr again rejects a custom DefaultDeleter as upstream requires. Utility alignment/division helpers retain upstream debug assertions.

Latest strict all-module suites passed fifteen binaries with LLVM 23.1.3 (17.05 seconds after the DISK guard) and GCC 16.1.0 (28.88 seconds). The features-without-DNN LLVM suite passed fifteen binaries in 42.03 seconds, including the actual SIFT case. Positive and rejected makePtr probes passed on both compilers using their real compile commands; the GCC probe uses the recorded build directory for gcm.cache and explicitly includes the standard memory header needed by imported shared_ptr instantiation. CI runs the generated-conversion drift check and both compilation contracts; LLVM additionally tests the features-only configuration.

The latest all-module Clang audit resolves 3,214 listed exports through the root and all sixteen direct modules. It inventories 3,382 entities, 169 not in export lists and 121 not-in-facade headers. These include manual adapters/private/device headers and genuine remaining portable helpers; no complete-API claim is made. The new docs/coverage.md keeps the four required matrices separate and lists remaining DNN shape/persistence convenience work and distribution gates explicitly.

Godot PR #1 head 40f72d5 has all three native jobs green in run 40053798222, including its real standalone math template and exact Windows extension export check. The organization has seven -m repositories; all seven now have concise root READMEs and runnable default templates. Source/template CI does not alone prove index/release readiness.

## DNN public shape-helper checkpoint

Restored shape_utils.hpp, which had incorrectly been described as internal in the old prune list. _Range keeps its SDK class identity. A generated named-module adapter retains twenty upstream overload bodies for slicing, planes, shape construction, products/concatenation, axis/range normalization, all-one checks and generic formatting/printing. Calls between adapters are explicitly qualified so exported templates do not refer back to TU-local global-module functions; public cv::dnn names remain available alongside the pinned ABI namespace. The generator preserves the upstream license and is drift-checked in native CI. Vendor bytes are unchanged.

LLVM 23.1.3 and GCC 16.1.0 strict all-module suites each passed sixteen binaries (13.71 and 30.37 seconds). The new direct-import tests check all four slicing overloads, shared tensor storage, Mat/UMat/array/integer shape overloads, valid/invalid axes and product ranges, generic vector formatting, print output and returned stream identity. The latest Clang audit resolves 3,215 export-list names through root/all sixteen direct modules and inventories 3,393 entities, 179 not in using lists, 120 not-in-facade headers and fourteen private implementation headers. Adapter names are not using-list completeness evidence and are tested separately. Persistence geometry conveniences and remaining portable headers still need explicit review before a complete-API claim.

## Static-implementation notice deployment checkpoint

The first immutable OpenCV archive 52f4c8f (SHA256 459e59767a7f75fa68abc15ee6f7ac031cdd98e59bb98665d7588e90e6ae22f9) passed an actual installed strict image/scalar consumer in 67.22 seconds, including exact grayscale pixels, PNG/JPEG roundtrips and the restored conversion/min-max/makePtr contracts. The actual default template installed in a fresh home also built strictly and wrote its 64x64 PNG. Its release/relocation audit is running separately.

Review of the read-only mcpp packing implementation confirms automatic notice discovery walks shared-library locations, not vendored static implementation sources. The generated OpenCV manifest now explicitly deploys all fourteen retained wrapper/upstream license and copyright files with original relative paths; this avoids basename collisions and does not change the engine. The manifest generator is deterministic, every source exists and all destinations are unique. The index's actual archive/template deployment test checks all notice bytes and PNG dimensions after running the relocated executable. Final archive/native/deployment checks for this packaging checkpoint remain required.

## Persistence, optimizer and SDK exception-runtime checkpoint

Restored the FileStorage geometry/vector/enum/stream helpers previously listed as missing. The generator retains 65 pinned public inline bodies and the original license, adds typed forwards for the SDK's non-template priority cases and copies only private vector proxies into a module-private namespace. Internal calls use qualified adapter lookup; custom serializers alone fall back to ADL. SDK FileStorage, FileNode, iterator and external WriteStructContext keep their original identities. The enum constraint keeps the SDK's optional void* template argument, including explicit <Mode,nullptr> calls, without exposing a dependent enable_if pointer type that crashed LLVM23 module import. No upstream or engine source changes were made.

Independent native-header and module executables now compare XML/YAML/JSON and all three Base64 forms byte-for-byte; ordinary cases are 432/638/643 bytes and Base64 cases are 722/982/890 bytes on the verified Linux compiler profiles. Tests cover scalar saturation, geometry, Matx, nested vectors, DMatch legacy priority, default/malformed nodes, iterators, enums and user-defined ADL serializers. GCC16 and LLVM23 comparisons pass. A direct geometry import also restores the public optimizer extension header; a user backend verifies the SDK's rejection and unsupported-geodesic report/dispatch contracts.

Latest strict all-module suites passed nineteen binaries on LLVM23.1.3 (24.33 seconds) and GCC16.1.0 (38.36 seconds). Both compilers' positive/negative makePtr contracts pass; the baseline guard checks 33 interfaces. The Clang inventory resolves 3,217 listed exports through the root/all sixteen direct modules, with 3,395 entities, 179 not in using lists and 119 not-in-facade headers. Manual adapters are exercised separately and not inferred from list counts.

Source CI on 5da7fc8 passed Windows/macOS/GCC/musl, but LLVM22 found uncaught cv::Exception only in the ORT process. A retained LLVM23 reproduction failed the typed error-catch test under the default static exception runtime. Changing only the Linux glibc target to cxx_runtime=toolchain-coupled made the same staged ORT project pass all sixteen binaries in 11.61 seconds. The source generator now declares that contract, matching the existing standalone ORT project and ORT template. The named template additionally checks a typed OpenCV exception after successful CPU Add inference; the actual rendered/strictly built template passed locally. The 3.8 GiB completed diagnostic project was removed after preserving logs, compile/link metadata and ELF evidence.

The current native matrix and immutable archive/index gates must validate this new checkpoint; the 5da7fc8 results do not prove it. gRPC 0d11b22 now has all Linux/macOS native checks and its actual template CI green. Remaining portable-header/API classification and broader codec/backend acceptance remain visible in the coverage matrices; nothing is merged or released.

The current coupled-runtime ORT candidate passed all nineteen strict test binaries with LLVM23 in 138.66 seconds (114.50 seconds build). Generated build inputs now preserve unchanged bytes and modification times: regeneration preserved 134 files, and the same GCC persistence check completed in 1.94 seconds after its prior 202.64-second rebuild. This avoids needless SDK recompilation and reduces local resource pressure.

The resource ceiling remains mandatory for every local heavy check: the shared slice caps total task CPU at sixteen cores (50% of this 32-thread host), memory at 32 GiB (about 52% of 62 GiB), with a 28 GiB pressure threshold. CPU affinity is additionally enforced with taskset 0-15 because this user slice does not delegate cpuset. MCPP_JOBS=3, serial heavy checks and available-memory/disk monitoring reserve room for other user applications; unrelated processes are never stopped.

Index updater run 40062594528 at 9f41559 completed all 28 candidate jobs successfully across the applicable Linux/GCC/LLVM, Windows and macOS combinations; scan passed and propose was intentionally skipped on the PR. This verifies that reapplying the candidate index after registry refresh fixes the actual published-index overwrite. The upcoming OpenCV source/archive checkpoint still needs its own checks.

## Portable HAL, instrumentation and tracking checkpoint

The exact 7e4fe39 immutable source archive (SHA256 1248a3fabfc7116941656c476389e86c7654b229570bc1d00ff4943b6eb1afbf) passed fresh index installation, strict actual default-template build, release packing, relocation and execution without the original home/runtime paths. All fourteen retained wrapper/upstream notices match byte-for-byte. Index fdac79a pins that source; current source/index native CI is still running.

Further public-header review found four real portable omissions: core/imgproc HAL, the instrumentation external interface, and the documented tracking extension header. The adapters now expose 172 core HAL names, 34 imgproc HAL names, original instrumentation types/state functions and ten tracking classes/aliases. SDK @cond IGNORED compatibility helpers/private filter classes are not added. Instrumentation remains disabled in the current SDK configuration, and behavior checks require the original no-ops rather than inventing collection support. setFlags keeps both integer/enum calls by forwarding to the original external enum function; NodeData equality retains its SDK entity.

New behavior checks exercise saturating HAL arithmetic, Hamming/L1 norms, exp and image color conversions; instrumentation construction/timing/copy/equality and disabled-state calls; downstream-derived tracking features dispatched through the original feature set, target-state accessors and sampling. The first sampler expectation was corrected after reading the pinned implementation: it always returns false even when samples are populated. The wrapper preserves that behavior and checks the generated samples independently.

LLVM23.1.3 strict all-module validation passes twenty-two binaries in 19.01 seconds. Baseline ISA checks pass all 33 interfaces and both makePtr compilation contracts pass. The exact Clang audit resolves 3,454 listed names through root/all sixteen direct modules, inventories 3,645 entities with 202 not in using lists, 115 not-in-facade headers and fourteen private implementation headers. Unlisted counts include manually adapted and SDK-ignored compatibility entities; these counts do not prove full API behavior. GCC16.1.0 strict all-module validation also passes twenty-two binaries in 44.49 seconds. Remaining header classification, point-cloud detail helpers and the wider codec/backend acceptance matrix remain open.

The first GCC attempt was terminated by the task guard when available filesystem space crossed its 6 GiB floor; it is not counted as a pass. Only task-owned generated outputs were removed after verifying no running process referenced them and no tracked files were involved: completed LLVM/stopped GCC test executables and completed Godot/llama/index build trees. Logs, source, compiler metadata and the model fixture were retained. The successful retry used three jobs to reduce simultaneous link temporary files, under the unchanged sixteen-core/32 GiB slice limits.

## WebP codec integration checkpoint

Reuses the existing compat.libwebp 1.5.0 source recipe rather than importing a second implementation. Index 7e5a54c adds independent demux/mux features and explicit COPYING/PATENTS/AUTHORS deployment; all historical platform resource blocks remain byte-identical. Strict isolated LLVM23 consumers pass a genuine two-frame lossless animation/EXIF container test plus the original encode/decode roundtrip. The unchanged-default variant passes independently, and its compilation database excludes mux/demux units.

The OpenCV imgcodecs-webp feature implies imgcodecs and requests both provider features. HAVE_WEBP is private to imgcodecs implementation paths, including the retained Windows TU adapters; generated configuration/build information reflects selection. A generated-input consumer tests lossless transparent RGB preservation, alpha, exact animation pixels/timing/loop count and XMP metadata bytes. Focused strict runs pass LLVM23 (72.43 seconds) and GCC16 (134.31 seconds). LLVM22.1.8 with all-modules,imgcodecs-webp passes all twenty-three binaries in 252.78 seconds, including the persistence/SDK oracle, HAL, tracking and DNN cases.

Source CI now pins the codec/ORT index fixture before codec tests and adds native WebP verification on all five target/toolchain jobs, with an explicit musl triple. Linux LLVM22 additionally checks codec-off behavior and the absence of the optional dependency from lock/compilation records. The candidate tool accepts a focused binary filter and explicit target to avoid creating unrelated executable copies for each backend check. The first disabled check passed actual codec behavior but exposed that mcpp emits no lock for an empty production graph; the helper now checks an optional lock plus actual compile units, instead of requiring a nonexistent file. The corrected strict rerun passed in 86.10 seconds and proved compat.libwebp absent from the production lock/actual compile-unit records.

The latest stable mcpp was rechecked through the upstream release API and remains v2026.10.24.1 (published 2026-10-23T22:02:51Z). Tool pins remain current; engine source is read-only. Native CI, final immutable archive/consumer/deployment checks and the wider API/header/backend ledger still apply. No merge/tag/release is created.

The continuing API audit identified concrete remaining static-inline conveniences beyond the count-based inventory: morphologyDefaultBorderValue, initWideAngleProjMap overloads, imwritemulti, raw array wrappers, alignment/norm/Jaccard helpers and several public scalar/constant aliases. These must be restored or individually justified; the old prune comments are not accepted as completeness evidence. Portable point-cloud detail rendering helpers and plugin descriptor types also remain under review. This WebP checkpoint does not close those API gates.

## Portable inline and value-operator restoration checkpoint

Restored fifteen pinned core helper bodies (six array norms, five alignment overloads, rectangle Jaccard distance and three raw-array wrappers), morphologyDefaultBorderValue, both enum/integer initWideAngleProjMap overloads, imwritemulti, original global cvTrunc/cvRound64, utils::getThreadID and the ArUco default threshold. Direct image tests exercise morphology defaults, wide-angle map equivalence, actual BMP file dispatch and threshold value/address/SDK detector initialization.

An independent native-header executable exposed the upstream std::array rawInOut typo: its member lacks static, so the free SDK wrapper cannot call it without an instance. The adapter now delegates through an instance to the original member, retaining vector semantics and original array storage/type behavior; the reference consumer uses that same SDK member explicitly. No vendor or engine source is changed.

The SDK oracle then reproduced a real old adapter defect: a legitimate rectangle near INT_MAX gave Jaccard distance 1 instead of the SDK's 0.875, because the handwritten intersection overflowed endpoint arithmetic. All 122 types.hpp free operators are now generated from the pinned source, retaining overloads, compound operations, scalar hypercomplex arithmetic and robust intersection. The before log is target/portable-rectangle-before.log; after restoration the independent helper/member/module outputs match at 222 bytes on LLVM23 and GCC16, including arithmetic, empty/wide unsigned norms, alignment, raw-vector/array identity, geometry/compound operators, soft-scalar rounding and thread identity.

Strict all-module suites pass twenty-six binaries on LLVM23.1.3 (4.26 seconds after the final fixture correction) and GCC16.1.0 (66.06 seconds). Positive/rejected makePtr contracts and 33 baseline interface checks pass both. Clang verifies 3,457 listed names through root/all sixteen direct modules; it inventories 3,645 entities, 199 unlisted names, 115 not-in-facade headers and fourteen private implementation headers. Manual operators/helpers are tested separately from listed-name counts. LLVM22.1.8 also passes all twenty-six binaries in 242.28 seconds; its independent SDK/helper comparison matches the same 222 bytes and both compilation/baseline checks pass.

The SDK's TU-local ArUco constant cannot be exported with a using declaration. Its module value keeps cv::aruco::DEFAULT_VALID_BIT_ID_THRESHOLD and matches the SDK detector. A captured compile probe confirms that including the original header and importing the module makes that same value name ambiguous; module-only value/address access passes LLVM/GCC. The existing full-core GCC header-mixing limitation remains separate.

The old prune loop no longer rewrites API lists. It builds once or replays a log, returns failure and writes explicit JSON/log diagnostics. Every reviewed using exception requires a reason and the generated notes retain it. Replaying the actual ArUco export failure identifies its recorded declaration while preserving every curation/export file byte. Remaining public fast-math globals, matrix/vector overload completeness, portable point-cloud/plugin helpers, device declaration adapters and unloaded-header classification still need review; full-support/release readiness is not claimed.

A separate staged LLVM22.1.8 dnn-onnxruntime build passed all thirteen CoreCppTypes cases, including typed OpenCV exception capture, in 188.10 seconds (165.76 seconds build). Log: target/portable-types-ort-llvm22.log. The previous remote source checkpoint 16baa03 currently has GCC, Windows, macOS and musl native jobs green; its LLVM22 ORT/template step is still live. These are separate from the upcoming helper/operator checkpoint.

Final distribution review must also account for embedded legacy source/header licence preambles, beyond the fourteen named OpenCV notice files and selected provider notices already checked. COPYRIGHT alone does not contain those complete preambles; aggregate and verify them before claiming complete notice coverage. This remains a release gate rather than being hidden by the existing byte-identity notice checks.

## Local resource guard clarification

All local heavy checks share the existing capped slice and run serially with
MCPP_JOBS=3. The local runner samples whole-host CPU and available memory every
second: it refuses to start at 80% CPU or below 20% available memory and stops only
its own test process group when those thresholds are reached during a run. The
16-core quota and 32 GiB memory limit additionally cap this task's aggregate
usage. Host sampling is reactive and cannot guarantee that unrelated processes
never cause a brief whole-machine spike; unrelated processes are never stopped.

## Embedded source notice deployment checkpoint

Source CI at 16baa03 completed all five native jobs successfully, including the
Linux LLVM22 ORT path. Later helper/operator changes still need their own CI.
The distribution audit found that named license files alone omit complete older
BSD preambles embedded in SDK headers. tools/generate_source_notices.py now
preserves 542 distinct copyright/license comment blocks at 2,077 retained source
locations in a deterministic 791,266-byte supplement. It skips quoted strings,
retains original comment bytes and records source locations. Named notice files
remain separate. All 15 OpenCV notice artifacts are declared by runtime.deploy;
WebP consumers additionally receive the provider's three notices.

The generator's drift check runs in every native CI leg. Local checks confirm
complete base/types/matx/mat.inl BSD preambles, all 15 deployment inputs, and all
2,305 retained SDK files matching the pinned upstream archive. Index relocation
acceptance now explicitly requires the supplement byte for byte. Actual archive
installation/relocation and native CI for this checkpoint remain pending.

## Global fast-math API checkpoint

The remaining public global fast-math overload sets cannot be exported with a
single using declaration because they mix TU-local platform implementations and
external SoftFloat functions. Twenty exact forwards now preserve the original
cvRound/cvFloor/cvCeil, cvIsInf/cvIsNaN and CV_FOURCC names and signatures. Private
bridges call original SDK overloads; constrained public templates also coexist
with textual headers. Generator assertions pin all thirteen primitive overloads,
six SoftFloat overloads and the original FOURCC declaration.

All 26 strict all-module test programs passed with LLVM23 (21.52s), GCC16
(62.59s) and LLVM22 (109.69s). Each compiler's independent header/module oracle
matches exactly at 380 output bytes. The extended fixture checks negative/half
rounding, SoftFloat agreement, integer limits, Inf/NaN, overload addresses and
high-bit FOURCC bytes. The mixed-header test also passes on all three compilers.
Logs: target/fast-math-oracle-llvm23.log and target/fast-math-all-compilers.log;
complete compilation databases are saved under target/api-audit/fast-math-all-*.
Only completed task-owned executables were removed between compiler legs;
objects, BMIs and evidence remain. All heavy checks ran serially in the capped
slice with three build jobs and the host pressure guard.

The earlier 019eaadb immutable archive passed fresh-home WebP build/run,
release packaging and relocation with eighteen byte-identical notices. Its
source native matrix was still queued at the last authoritative poll; index
2e54735 carries that verified archive. Final source/index alignment, native CI
and the remaining portable-header/codec/backend inventory are still required
before merge/release readiness can be claimed.

## Preview plugin ABI and CPU point-cloud public API

The facade now includes the original llapi/llapi.h preview ABI declarations and
ptcloud/detail/kinfu_frame.hpp CPU rendering functions. Six original global
ABI types/enumerators and two point-cloud functions are exported by using
declarations; no copied ABI structures or backend implementations are created.
The upstream preview/no-backward-compatibility status is retained. Consumer
checks exercise typedef/tag identity, structure fields and size, enum values,
colored/shaded rendering, invalid points and typed invalid-input exceptions.

All 26 strict all-module programs pass LLVM23 (88.51s), GCC16 (72.49s) and LLVM22
(26.66s). Independent SDK/module utility outputs match exactly at 400 bytes on
all three. The compiler-derived audit resolves 3,465 listed root exports and all
16 direct module surfaces; inventory is 3,653 entities, 199 absent from using
lists (including adapters) and 113 headers not in facades, plus 14 implementation
headers. Remaining headers require explicit classification; these counts are
not a declaration of complete support. Logs: target/public-abi-rendering-all-compilers.log
and target/public-abi-rendering-api-audit.log. The initial local curated-name
spelling error was corrected; generation now rejects malformed C++ names early.
The failed initial run is not acceptance evidence. The read-only export
diagnostic regression still passes and no API was automatically pruned.

## CPU-usable CUDA helpers, HAL and explicit header decisions

Eight generated cuda.hpp helper overloads preserve original SDK bodies or exact
external function forwards. They restore createContinuous/ensureSizeIsEnough
for ordinary CPU Mat, GPU-returning overload signatures and header-only swaps.
Ten Event::CreateFlags operators are generated from the pinned CV_ENUM_FLAGS
macro definitions, preserving enum results and compound assignments. The SDK
oracle checks non-contiguous CPU input, continuous reshape, storage reuse/growth,
empty GPU header swaps, HostMem allocation mode swaps and every flag operation.
No GPU allocation or device execution is claimed. Original cvhalKeyPoint and 26
feature/imgproc HAL macro values are exported/mapped. Typed CV_FOURCC_MACRO
preserves integer/unsigned expression types, checked in constant expressions.

The initial all-module run was terminated by the host resource guard and is not
a pass. Later runs reduce the shared slice to CPUQuota=800% (eight of 32 cores),
CPU affinity 0-7 and MCPP_JOBS=2; memory high/max stay 28/32 GiB. The runner now
records CPU, available-memory and disk metrics when a protection threshold fires.
Only completed/stopped task-owned binaries are cleaned; user processes and
original files are untouched. This stricter cap is the current local default.

The capped rerun passed all 26 strict programs on LLVM23 (18.40s), GCC16 (80.77s)
and LLVM22 (32.75s). Each independent SDK/module utility output matches exactly
at 424 bytes. Log: target/cuda-hal-all-compilers-capped.log. Compiler-derived
lookup resolves 3,466 listed root exports and sixteen direct module surfaces;
inventory is 3,654 entities, 199 not in using lists and 112 headers not in
facades, plus fourteen implementation headers. Log: target/cuda-hal-api-audit.log.

The initial 113-header gap list is now explicitly reviewed in
port/api/header-review.toml. A coverage checker is included in native inventory
CI and does not edit exports or erase implementation gaps. Its full-support
acceptance mode correctly refuses the 23 remaining public-gap records: universal
intrinsics, public core dump utilities (not the ignored binding tests), and
original legacy global enum spellings. SDK/platform and implementation headers
have separate source-based reasons; classification does not make them supported.
These remaining public APIs, final archive/index alignment and exact-head CI
still prevent a complete-support/release-readiness claim.

The minimal default-feature macro consumer also passed all four tests with LLVM23 (40.39s); no algorithm module was enabled to obtain the additional HAL values.

## Windows notice ordering regression

Native 27d0b50 Windows CI failed the embedded notice drift check before
compilation. The repository already forces generated text to LF and preserves
upstream bytes. The actual defect was WindowsPath case-insensitive ordering
versus PosixPath ordering. Explicit case-sensitive relative path components now
produce one order on both hosts without changing the existing notice artifact.
A regression test emulates Windows path comparisons, preserves CRLF comment
bytes and rejects apparent license comments inside ordinary/raw string literals.
It reproduces the old failure in an isolated copy and passes with the fix.
The original 791,266-byte notice supplement still passes its drift check.
The new Windows native CI result remains required.

## Public core diagnostics and original legacy enum checkpoint

Twelve public static-inline dump functions now forward to the original SDK
implementations through ordinary module-private functions, avoiding transcribed
formatting logic. Eight external array/vector/cache diagnostic functions retain
their original declarations. The explicitly ignored binding-test region is not
exported. Tests compare scalar/string/geometry/criteria/range/vector formatting
and all four array diagnostic variants between independent native-header and
module consumers; the cache accessor signature is checked without touching user
cache directories. The original InpaintingModes type and six photo/video global
constants are exported unchanged, with tests against upstream C++ equivalents.

All 26 strict programs pass LLVM23 (27.99s), GCC16 (80.25s) and LLVM22 (31.03s);
each independent SDK/module utility output matches exactly at 1,302 bytes. Log:
target/utils-legacy-all-compilers.log. Compiler-derived lookup resolves 3,481
listed root exports and sixteen direct module surfaces. Inventory is 3,699
entities, 229 not in using lists (including adapters/ignored SDK helpers), 109
headers not in facades and fourteen implementation headers. Log:
target/utils-legacy-api-audit.log. Header review now has twenty remaining
public-gap records, all related to universal/architecture-specific intrinsics.
An implemented header review also requires the compiler inventory to confirm
that its facade actually loads that header. No review label hides these gaps.

The Windows job for 11aa896 has passed Embedded upstream notices match retained
sources and progressed through the WebP stage into image codec feature tests at
the last authoritative poll. Its whole job and the new implementation head
still need final native CI. The case-sensitive path ordering fix has therefore
passed the real Windows notice gate, not just local emulation.

For the next SIMD work, an isolated Clang module experiment disproved namespace
using-directive-only exports: qualified calls remain invisible after import.
Explicit using declarations for original v_float32x4/v_load/v_setall_f32/v_add/
v_store pass the same import-only probe without copied SDK vector types or ISA
flags. Logs: target/simd-namespace-probe.log (expected failed experiment) and
target/simd-explicit-using-probe.log (successful explicit-name experiment). This
is prototype evidence only; it does not certify the unimplemented SIMD family.

The isolated SIMD prototype additionally passed actual managed mcpp strict
build/run with LLVM23, GCC16 and LLVM22 (not only syntax checks). It uses the
original SDK vector type/load/setall/add/store through explicit using
 declarations; no copied vector type or private ISA flags are introduced. The
first temporary manifest lacked an explicit binary entry and was corrected;
only target/simd-native-probe-managed-final.log is runtime acceptance evidence.
Prototype source/manifest path: /tmp/mwrap-simd-native-probe-0pbdluu1. The twenty
SIMD family records remain public gaps until the full surface and target matrix
are implemented and tested.

## Native 128-bit SIMD module checkpoint

The optional simd feature adds core.simd plus a separate detail.simd SDK adapter
and selected root re-export. Original SDK vector types and 220 enabled names
are available. The frozen SDK declaration inventory drives reviewed generation;
SSE-specific and FP16 names keep their target guards. Fifteen typed SIMD
capability macros describe the compiled library baseline. Target validation
rejects unsupported architectures; no empty emulator or wider-ISA feature is
registered. READMEs stay at 51 lines per language; detailed usage is docs/simd.md.

An initial combined-GMF/import form reproduced GCC's SVD default-argument
conflict; separate header adapter/public aggregation resolves it without SDK
patches. A deliberately unaligned input then exposed GCC importing an intrinsic
inline body with incorrect alignment behavior: native SDK passed, module code
segfaulted at _mm_loadu_ps. Ordinary producer-compiled forwards for the memory
entry overloads preserve original SDK operations and pass unaligned load/store.
One uint/unsigned-int forward redeclaration is normalized as the same overload.
No API was removed based on compiler failure; vector classes are not copied.
Logs: target/simd-gcc-backtrace.log, target/simd-module-gcc-memory-dedup.log.

Final native SDK/module outputs match exactly at 495 bytes on LLVM23/GCC16/LLVM22;
220 names resolve through direct and aggregate imports on each compiler. Log:
target/simd-native-oracles-verified.log. Full all-modules,simd composition passed
28 programs on LLVM23 (319.17s), GCC16 (587.71s) and LLVM22 (354.02s), with SIMD
and utility SDK outputs matching 495/1302 bytes, 35 baseline interfaces and
makePtr positive/negative checks. Only completed own binaries were cleaned
between compiler legs. Log: target/simd-all-modules-composition.log. GCC release
parity and surface checks pass (94.26s), including deliberately unaligned memory;
log: target/simd-release-gcc.log. All heavy checks were serial under the shared
eight-core/32 GiB slice, two build jobs and whole-host pressure guard.

The compiler inventory now includes enabled optional facades and transitive
namespace imports. A regression checks nested hal_baseline/simd128_cpp lookup.
All five inventory unit tests pass. With SIMD selected, import-only probes
resolve 3701 root names plus sixteen direct algorithm modules, while the SIMD
checker separately covers direct/root SIMD imports. The inventory has 3948
entities, 255 not in using lists, 102 not-in-facade and fourteen implementation
headers. Log: target/simd-composition-api-audit.log. Counts are not overload
behavior proof. Header review retains eight partially accepted native records
and twelve unimplemented mode/ISA records; full-support mode refuses both.

Windows 6d60a93 failed image_helpers compilation in Clang23 MSVC ABI while
resolving the ArUco threshold constant (all other 25 programs passed). A generated
explicit float token now replaces the imported deduced-auto/private initializer
chain, retaining the exact pinned SDK value and qualified name. The four image
helper contracts pass LLVM23/GCC16/LLVM22 after this change (4.71s/8.67s/4.91s).
Log: target/aruco-explicit-float-three-compilers.log. Actual Windows confirmation
is still required; no test or export was disabled. Native CI and final source/
archive/index alignment remain open; no merge/tag/release readiness is claimed.

## Software SIMD implementation checkpoint

`simd-emulated` implies `simd` and selects the original SDK's
CV_FORCE_SIMD128_CPP implementation in the public SIMD/macros adapters only.
The package feature union selects one vector implementation; algorithm kernel
ISA/dispatch settings are unchanged. Original v_reg<T,N> definitions and SDK
function bodies are retained. After including ordinary core declarations, the
adapter temporarily maps CV_INLINE to inline for namespace function templates
and restores the macro. Software GCC memory templates are exported directly,
without the native memory-call adapters.

Independent original-header and import-only consumers produce identical
644-byte results on LLVM23, GCC16 and LLVM22. Generic eight-lane arithmetic,
bitwise operations, double conversion halves, reductions and rounding shifts
supplement the native fixture. All 223 enabled names resolve through direct and
aggregate imports. GCC release parity also passes (90.89s); the three debug
checks took 2.35s after initial compilation, 73.94s and 47.65s. Switching back
to native mode passes LLVM23 (40.76s) and GCC16 (72.39s), preserving the original
495-byte output and 220 names. Logs: /tmp/mwrap-simd-emulated-llvm23.log and
/tmp/mwrap-simd-emulated-matrix.log.

The generic SDK v_reinterpret_as_uint refers to a missing trait member.
Both independent original-header and import probes fail with reinterpret_uint,
while its declaration remains exported. This is a recorded upstream behavior,
not a deleted export or a claimed successful instantiation. Header review keeps
one software record partial, eight native-family records partial and eleven
other ISA/legacy headers open; full-support acceptance still rejects twenty
unfinished records. Three-compiler all-main-module composition is running at
/tmp/mwrap-simd-emulated-composition.log. LLVM23 has passed all 28 programs
(314.59s), 644-byte SIMD/1302-byte utility parity, 35 interface argument guards,
makePtr positive/negative contracts and 223 direct/root SIMD names. GCC16 and
LLVM22 composition results remain pending. Native CI now includes software SDK
parity on every leg and optimized software parity on Linux GCC. Those new gates
require their own actual results before acceptance.

Index checkpoint 6ff350a registers namespace-only view preparation in workspace
CI, removes stale complete-index categories and adds a repeated rebuild
regression. A fresh-home immutable ORT consumer passed strict inference/public
API tests (103.25s). Index publication bytes/layout are unchanged. The index PR
and source PR descriptions now distinguish current source work from the older
verified immutable OpenCV archive. mcpp engine sources remain read-only.

## Exact Matx/Vec template API restoration

The SDK accepts determinant<float,3>(Matx33f); the former whole-operand wrapper
rejects it with too many template arguments. Eight adaptations lost original
argument forms: determinant, matrix/vector comma initialization, mixed-type
matrix/vector += and -=, and matrix multiplication. Directly restoring their
parameter patterns inside the original header producer reproduces a Clang22
frontend crash in api_surface_test. A separate import-only prototype passes,
so core now aggregates detail.core (original SDK facade/general adapters) and
detail.matx (exact Matx/Vec templates over already imported original classes).
No upstream file, engine or vector/matrix class definition is changed. Matx
arithmetic bodies retain their prior SDK accumulator/saturation behavior.

The independent original-header/import fixture exercises every argument prefix
of those eight families, explicit second-argument conversion for initializers,
function-pointer selection, saturation, derived-class determinant and matrix/
vector base deduction with exact SDK reference-return types. All three compilers
match 342 output bytes: LLVM22 2.45s, GCC16 73.41s, LLVM23 41.83s. Each also checks
SDK/import rejection of wrong determinant/product dimensions and const left
augmentation, rejecting compiler crashes as acceptance. Existing LLVM22 API/
Matx behavior passes after the producer split. Logs:
/tmp/mwrap-matx-template-matrix.log and /tmp/mwrap-matx-split-llvm22.log.
The failing direct-definition reproduction is retained at
/tmp/mwrap-matx-original-shapes-llvm22.log.

The preceding d1c3152 software SIMD full compositions completed successfully on
LLVM23 (314.59s), GCC16 (593.65s) and LLVM22 (357.54s): 28 programs each, 644-byte
SIMD/1302-byte utility parity, baseline interface arguments, makePtr contracts
and 223 SIMD names. After the Matx split, a new three-compiler all-modules,
simd-emulated run checks 30 programs and an additional 342-byte Matx oracle at
/tmp/mwrap-matx-split-composition.log. Native SDK/compiler/platform acceptance
still needs the exact new source head, and final immutable archive/index pins
are not yet aligned. No release/merge readiness is inferred from focused tests.

The new composition has passed all 30 programs on LLVM23 (69.62s) and GCC16
(156.35s), with all three output pairs identical, 37 interface argument checks,
makePtr contracts and 223 SIMD name lookups. LLVM22 is running on the same
source. The temporary detached worktree used to prepare the failed direct-shape
candidate is task-owned and can be removed after this checkpoint; its failure
and successful imported-type reproductions remain in the logs above.

## Explicit SIMD compatibility operators

With simd enabled, core.simd.legacy explicitly imports the compatibility header's
21 original cv::hal operator templates. Base/root imports keep those operators
out of their visible API; the test checks that boundary. The wrapper does not
copy operator bodies or add another implementation owner/feature. Its generator
extracts the exact operator list from the pinned original header.

Independent SDK/import fixtures match 976 output bytes in native and software
modes on GCC16, LLVM22 and LLVM23; software GCC release also passes. The fixture
covers all 21 names, normal syntax, explicit function-pointer selection,
compound reference identity and byte saturation. Native mode checks floating
complement; the original software SDK declares v_not<float,N> without a body,
so its floating complement and the unchanged module operator both fail to link.
That original failure is retained at /tmp/mwrap-simd-legacy-sdk-float-not.log;
software positive tests exercise integer complement. No broken export is pruned.
Focused logs: /tmp/mwrap-simd-legacy-llvm23.log and
/tmp/mwrap-simd-legacy-matrix.log.

Full all-modules,simd-emulated composition has passed all 32 programs on LLVM23
(297.73s), including SIMD 644, utility 1302, Matx 342 and legacy 976 byte-pairs,
39 interface argument guards, makePtr contracts and 223 SIMD name lookups.
GCC16/LLVM22 are running at /tmp/mwrap-legacy-simd-composition.log. Native CI
and exact final archive/index alignment remain acceptance gates.

The inventory selector now uses the active root's feature flags, ignoring stale
optional source entries retained in a CDB. Active SIMD requires both actual
adapter commands and includes the explicit legacy facade in its inventory.
Seven parser/profile regressions pass. Header review retains legacy as partial,
with eight native-family partial records, two software/legacy partial records
and ten other ISA records open. This is not full-support acceptance.

Further unlisted-entity review has identified explicit follow-ups for FP state
controls, videoio opaque forward declarations and generic SIMD broadcast
helpers. Source evidence and names are recorded in
 target/api-audit/unlisted-next-review.json. Existing aliases or using-list
counts do not prove those original qualified APIs are visible or callable.

## Qualified FP, opaque videoio and generic broadcast APIs

Restored six original cv::details FP state/function/RAII names and the original
IVideoCapture/IVideoWriter forward declarations. They use export-using with no
new implementation or class copy. The SDK/import oracle checks exact signatures,
64-byte state layout, direct save/set/restore, both RAII constructors, NOOP scope
and per-thread restoration. Opaque pointers remain incomplete as in the public
SDK header. All three compilers match 21 bytes (LLVM23 5.46s, GCC16 9.70s,
LLVM22 5.61s); this does not claim GPU/device implementation behavior.

Type2Vec128_Traits, v_setall and vx_setall now retain the original SDK type
mapping and ten scalar specializations. The static-inline broadcasts call
producer-compiled exact SDK specializations; unsupported plain-char traits are
rejected in both SDK/import probes. All three compilers match native 1311-byte
and software 1460-byte SIMD oracles; GCC software release passes too. Generic
broadcast function-pointer checks also run in the full composition. Call-boundary
performance parity is unmeasured. Legacy operator output remains 976 bytes.

Before the iterator-scope correction below, all 34 programs and five independent
SDK pairs passed on LLVM23 (75.31s), GCC16 (167.16s), LLVM22 (81.14s), alongside
39 interface guards, makePtr contracts and 226 software SIMD names.
Logs: /tmp/mwrap-fp-state-llvm23.log, /tmp/mwrap-fp-state-matrix.log,
/tmp/mwrap-broadcast-matrix.log, /tmp/mwrap-qualified-api-composition.log.

Native fb27853 Windows job 120871685646 then found two legacy fixture failures
(30 other programs passed). A broad using namespace cv::hal made the SDK's
unconstrained operator!= intercept MSVC array iterator comparison through its
base class. The fixture now imports only operator+, qualifies the other operators
and tests a derived/base equality iterator, reproducing the issue on Linux too.
A raw SDK Linux probe fails with the broad namespace and passes with the scoped
operator import: /tmp/mwrap-legacy-derived-iterator-repro.log. No SDK/operator
body was changed or test removed. Actual Windows confirmation remains required.

The corrected 34-program composition passes LLVM23 (24.13s). The following GCC
segment was stopped by the whole-host guard at 83.14% CPU, with 76.10% available
memory and 13.95 GiB free disk; it is not counted as a pass. Metrics are retained
in /tmp/mwrap-qualified-api-final-composition.log.resources.json. Once host CPU
returned to 4%, only GCC16/LLVM22 were restarted under a four-core quota and one
build job at /tmp/mwrap-qualified-api-retry.log. Other processes were untouched.

The first reduced-concurrency retry then found two zero-byte, non-executable
link outputs left by the interrupted linker. They were task-owned bin artifacts;
that run executed fourteen programs, left twenty unrun and is not accepted as
complete. Before relinking, host disk fell to 7.8 GiB and preflight correctly
refused further heavy work. Only the stopped task's GCC bin directory was
removed (9.5 GiB); object files, BMI caches, source and logs were retained.
The GCC verification now runs the same all-modules,simd-emulated configuration
one binary at a time and discards each executable only after a successful run.
All 34 unique filters and all five SDK output pairs are still required. This
bounds executable storage rather than narrowing test scope; progress and final
per-program results are recorded at /tmp/mwrap-qualified-batched-gcc.log and
/tmp/mwrap-qualified-batched-results.json. LLVM22 remains a separate required
post-correction run. Index 0861946's updater, OpenKAL and site checks pass;
its complete workspace validation remains pending.

GCC's bounded-storage run has now passed all 34 unique binaries (196.50s), with
all five SDK pairs identical (1460/1302/342/21/976 bytes), 39 interface checks,
makePtr contracts and 226 SIMD name probes. Its JSON result lists every program.
Only completed executables were discarded; features and test contents stayed
unchanged. LLVM22's final post-correction composition/inventory is running at
/tmp/mwrap-qualified-llvm22-final.log. Latest source/index native CI and final
immutable-archive alignment are still required before readiness.

LLVM22's corrected final run passed all 34 programs (30.20s), all five byte pairs,
39 interface guards, makePtr contracts and 226 SIMD names. The corresponding
fresh audit resolves 3715 root names and sixteen direct modules; it inventories
3953 entities, 247 not in using lists, 103 headers outside the active facade and
fourteen private headers. The new qualified/broadcast names are now accounted
for. Twenty partial/other-ISA header records and remaining entity classification
stay open; these counts are not a full-support claim. This local checkpoint is
ready for native CI, including the actual Windows fixture confirmation.

## Indexed becda22 archive and original-name audit

Index checkpoint 0d7f3d1 pins becda22229cb5ed43d7ce098291cf7a4e5f7d15b,
SHA256 67fdaec136e4acb4b0aa9d6a37c1c7952fa278180d82d6eab77dcac4211404ce,
on all three descriptor platforms. A fresh home resolves that exact payload,
renders the unmodified default template and passes strict build/run (4m31s),
release packing (4m38s), isolated relocated execution, ELF closure and fifteen
byte-identical wrapper/upstream notices. Log:
/tmp/mwrap-becda22-template-deployment.log. A second fresh home passes the
indexed image consumer and restored qualified FP signature/state checks
(301.68s): /tmp/mwrap-becda22-indexed-cpu.log. These certify that source archive,
not later uncommitted API changes. No release/tag/merge was performed.

Before the Mat assignment changes below, an import-only Clang22 consumer probes
all 247 original names absent from the using lists, with only the root module
and explicit optional legacy SIMD module imported. After mapping diagnostics,
the 135 successful names are recompiled together without the 112 rejected names.
Report: target/api-audit/unlisted-import-probe/report.json; raw attempt logs are
retained there. Name visibility does not certify overloads, signatures or runtime
behavior. The rejected list includes SDK-ignored compatibility and private/helper
entities, binding tests, plus real public omissions; it is not an assertion of
112 missing public APIs. Mat augmented assignments and DNN Arg equality/backend
utility declarations require follow-up. Other-ISA header review remains open.

## Mat augmented-assignment restoration

The name probe demonstrates that cv::operator^= is unavailable through the
previous module surface. The SDK operations.hpp defines 49 augmented-assignment
macro invocations, each with mutable and const left operands. A generator now
expands those exact bodies/signatures in detail.matx, where original SDK types
are imported rather than textually redeclared. Scalar/Mat/Mat_/Matx operands,
original template order and return-reference identity are retained. Non-template
SDK forms use the existing constrained dummy-template adaptation; class bodies
and retained upstream files are unchanged. CI checks reproducible generation
and compares independent SDK/module consumers.

The shared behavioral fixture runs all 49 operand families with both left-operand
qualifiers, then checks explicit typed/fixed-size signatures and selected function
pointers. SDK/module output is identical (1049 bytes) on LLVM23 (5.86s), GCC16
(12.84s) and LLVM22 (6.14s). Logs: /tmp/mwrap-mat-aug-llvm23.log and
/tmp/mwrap-mat-aug-matrix.log. The complete 36-program all-modules,simd-emulated
composition and all six SDK oracle pairs are now running under the shared
four-core/one-build-job resource limits at /tmp/mwrap-mat-aug-composition.log;
GCC retains bounded executable storage. Current source CI and final archive
alignment remain required after this restoration.

The complete restoration run now passes all 36 programs on LLVM23 (49.04s),
LLVM22 (59.80s) and GCC16 (254.96s, executable storage bounded). Each matches all
six independent SDK pairs: SIMD 1460, utilities 1302, Matx 342, FP state 21,
Mat assignments 1049 and legacy SIMD 976 bytes. All retain 39 interface guards,
makePtr positive/negative contracts, 226 software SIMD names and the original
negative trait/SDK probes. GCC per-program results:
/tmp/mwrap-mat-aug-batched-results.json. Generator check mode and byte/mtime
idempotence pass; no retained SDK source is modified.

The reusable tools/probe_unlisted_api.py verifies consumer/inventory feature
profile equality and uses BMI mappings only from that same profile. It is now
part of the Clang inventory CI step. Against the unchanged pinned-header
inventory and the new Clang22 BMIs, the report has 136 visible and 111 rejected
original names: operator^= is now visible. Output:
target/api-audit/mat-aug-unlisted-import/report.json;
/tmp/mwrap-mat-aug-unlisted-probe.log. This diagnostic does not turn unclassified
rejections into accepted exclusions. DNN equality/legacy backend utility names,
remaining entity classification, current native CI and final source/archive
alignment still require work before the requested final readiness report.

## Original DNN names and source-reviewed rejection inventory

Restored the original Arg equality function in both cv::dnn and its original
ABI namespace, plus six original Inference Engine utility declarations. No SDK
class or implementation body is copied. An independent SDK/module fixture checks
all six exact function signatures, equality/function-pointer identity, four
StsNotImplemented results and two original no-op cleanup contracts. Explicit
ABI-namespace lookup found that exporting only the public spelling was
insufficient; both spellings now designate the same original function.
The original incomplete cudev::GpuMat_ declaration is also exported and checked
as an opaque pointer, without enabling a CUDA backend.

With those changes, all 38 programs pass on LLVM23 (52.38s), LLVM22 (62.77s)
and GCC16 (272.28s, bounded executable storage), matching seven SDK pairs:
1460/1302/342/21/1049/92/976 bytes. Logs:
/tmp/mwrap-dnn-qualified-composition.log and
/tmp/mwrap-dnn-qualified-batched-results.json. The run retains 39 interface
guards, makePtr contracts and the existing negative SIMD/trait probes.

A final source review identified two software SIMD width enums as public,
rather than private helper names. Their exact SDK entities are now exported in
both public and original mode namespaces; selected-mode tests check values and
enum type identity. The subsequent four-program SIMD/legacy suites pass on
LLVM23 (8.29s), GCC16 (21.43s), LLVM22 (8.69s), with unchanged 1460/976-byte
oracles and 230 enabled name spellings (two enum entities, four spellings added).
Log: /tmp/mwrap-dnn-width-final.log. The 38-program timings above precede this
two-enum extension; final exact-head native CI remains required.

The final fresh Clang22 inventory resolves 3726 listed root names and sixteen
direct modules. Of 3953 entities, 238 lack using-list entries: 137 resolve from
imports, and 101 have exact source-reviewed exclusions (83 implementation/SDK
compatibility helpers and 18 language-binding tests). Each entry in
port/api/entity-review.toml has exact names, a reason, source SHA256 and literal
source evidence. The validator rejects new unreviewed names, stale exclusions,
changed source hashes and profile mismatches. Software-only helper entries are
conditional on simd-emulated. No blanket namespace exclusion is introduced.
Seven existing audit regression tests pass. This is review coverage, not a proof
of all visible overloads or runtime backends.

An additional independent ordinary-consumer probe then found a real overload
problem: SDK Mat_<float> + Mat_<float> compiles, but module-only lookup fails
because current Mat/MatExpr forwards constrain operands to exact base types.
Raw SDK/module logs are retained in target/api-audit/typed-mat-lookup/; the
observational probe reports sdk exit 0 and module exit 1. This is not counted as
a passing acceptance test. Original parameter shapes/conversions and MatExpr
assignment families require repair before full-support/merge/release readiness.
Other-ISA header acceptance and exact final archive/index alignment remain open.

## Mat expression parameter and conversion restoration

The SDK/module ordinary-consumer probe above is now positive on both sides.
All 64 Mat/MatExpr/MatShape forwards retain their original SDK parameter types,
with the existing constrained dummy-template adaptation for non-template SDK
functions. This restores ordinary overload ranking, derived Mat_ operands and
implicit numeric/user conversions; exact-base-type constraints no longer reject
valid SDK expressions. Each forward still invokes the captured exact original
SDK function. Sixteen original MatExpr augmented-assignment bodies and the
original deprecated Mat_ comma initializer are generated from mat.inl.hpp in
the imported-type producer. No SDK class/body or retained source file is changed.

The expanded independent oracle covers 114 assignment cases, exact signatures
and returned references, thirteen ordinary conversion cases and comma
initialization. SDK/module output matches 1574 bytes; the typed-Mat probe changes
from SDK 0/module 1 to SDK 0/module 0. The initial Clang22 focused test passes
without the earlier exact-Matx serialization crash.

The first full composition attempt is not accepted: the resource guard stopped
it when free disk reached 5.47 GiB (host CPU 7.54%, available memory 81.31%).
No external processes were stopped. Only task-owned untracked executable bins
were removed, after checking no live references; objects, BMIs, sources and logs
were retained. Disk free recovered from 5.3 GiB to 16.93 GiB. Every compiler now
uses one binary at a time, removing it only after its successful run.

The complete unchanged all-modules,simd-emulated test set passes all 38 unique
programs on LLVM23 (120.87s), GCC16 (270.10s) and LLVM22 (146.14s). Every compiler
matches all seven SDK pairs: 1460/1302/342/21/1574/92/976 bytes. All retain
39 interface guards, makePtr contracts, 230 enabled SIMD name spellings and
negative SDK/trait checks. Fresh Clang audit/probes still resolve 3726 listed
names, sixteen direct modules, 137 manual names and 101 exact source-reviewed
rejections; seven audit regression tests pass. Logs and per-program results:
/tmp/mwrap-mat-conversion-bounded-all.log and
/tmp/mwrap-mat-conversion-bounded-results.json. The stopped run is retained at
/tmp/mwrap-mat-conversion-composition.log with resource metrics beside it.

Index consumption is being extended with typed-Mat addition, exact MatExpr
assignment pointers and affine-expression values, so an older cached candidate
cannot stand in for this restoration. Final immutable archive alignment,
independent-home template/API/deployment tests, explicit optional capability
matrices and exact-head native CI remain required. On 2026-10-25, the latest
published mcpp is still v2026.10.24.1, matching all project/CI pins. No merge,
tag or release has been performed; mcpp engine source remains unchanged.
