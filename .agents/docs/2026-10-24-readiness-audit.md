# Implementation and release readiness ledger

The full objective remains the OpenCV 5/main C++ API and backend plan, ORT wrapper, executable -m templates, one checkpoint PR per repository, green relevant CI and ecosystem review. Nothing is merged or released by these checkpoints. mcpp engine code stays read-only; its existing documentation-only PR is not a prerequisite.

## Verified current checkpoints

- OpenCV remote head 02fb836: native CI run 40026710108 passed all five jobs, including core, image codec behavior and the real default project template; Linux GCC additionally runs DNN/unifont. Root LICENSE/metadata/readme now consistently say Apache-2.0. The image example explicitly requests required features.
- ORT head de3a0a9: native CI run 40022727603 passed Linux GCC/LLVM, macOS arm64 and Windows MSVC ABI. Each runs three module consumers plus the actual project template. LLVM audits the exact SDK declaration inventory. Supplemental official headers omitted from Windows SDK are source/hash pinned. The config-module internal-linkage adaptation and mixing limits are explicit.
- SDK provider: isolated SDK install plus LLVM/GCC inference/header tests passed; no wrapper/source cache was preseeded in that SDK test.
- ORT immutable archive: source de3a0a9da143f4ae52eb5e1a0302ba3fdf623b6d downloaded twice with SHA256 24d4bc26d2f426e3643f21d289d92e7a23b3ba54524a099a1079c1c490915251. Index consumers execute real inference and public helper/config APIs. The complete unchanged publication layout also passed in a fresh MCPP_HOME: actual archive/SDK/toolchain installation, mcpp new, strict build and unmodified template run.
- ORT Linux distribution check: a release-mode consumer installed the actual immutable indexed archive and official SDK, then `mcpp pack --mode self-contained --format dir --toolchain llvm@23.1.3` succeeded. Its copied bundle passed Identity inference from `/` with a fresh HOME and MCPP_HOME/XLINGS_HOME/LD_LIBRARY_PATH/LD_PRELOAD absent. The bundled loader's `--list` resolved every file-backed ELF dependency inside the relocated bundle. This verifies Linux LLVM relocation, not Windows/macOS or GCC deployment. The bundle's license collector copied the ORT LICENSE but omitted ThirdPartyNotices.txt; explicit notices staging remains a distribution gate.
- Index PR #564 contains the ORT wrapper descriptor, SDK provider, mwrap specification/skill and category/test discovery changes. The full updater audit passed (85 automatic declarations, one manual immutable snapshot), and all 14 updater adapter tests passed. Namespace-filtered views are test-only byte copies; normal publication layout is tested separately and is never converted.
- Llama PR #5 adds b10069.3 CPU template, keeping its legacy package/module identity. No-argument startup and a checksummed GGUF decode/sample passed through actual mcpp new consumers. Root LLVM build and all 116 existing unit/repository/build-helper checks passed. All eight checks on head 2de7714 passed, including Vulkan, Metal and Linux arm64. Its final archive/index descriptor is still required.

## Native window checkpoint (new implementation)

- Added highgui-native and explicit highgui-gtk3/highgui-win32/highgui-cocoa features, retaining API-only highgui and empty defaults. Target mismatches are errors. Current GTK SDK closure is Linux x86_64 glibc only; other Linux architectures and musl are refused.
- GTK resolves declared xim SDKs through a scoped pkg-config view. It does not fall back to host headers/libraries. Only the GTK implementation TU uses the pthread compiler mode, keeping public BMIs consumable without that flag.
- Bundled zlib uses upstream Z_PREFIX and Unix codec implementation symbols use hidden visibility, preventing their collision with shared GUI SDK dependencies.
- Linux LLVM and GCC strict tests passed all seven binaries, including actual native window creation, image display, event processing, geometry checks and destruction, plus PNG/JPEG roundtrips. The actual named window template also passed strict LLVM build and timed smoke execution.
- Win32/Cocoa implementations and their native CI steps are added, but have no passing CI evidence yet. These results must not be inferred from the earlier five green API/codec jobs. The template helper now restores package metadata reading and explicitly performs a strict build before running.

## Review decision

| Repository | Merge readiness against the requested scope | Release readiness | Index readiness |
|---|---|---|---|
| opencv-m | Not ready: incomplete main-module/API/backend scope; new native-window CI pending | Not ready for a full-support release | Final source archive descriptor and installed consumers absent |
| onnxruntime-m | CPU-only candidate has four green native jobs; final API/distribution review pending | No release/tag created; distribution notices and platform deployment verification pending | SDK/wrapper descriptors and Linux real archive/template consumption implemented in index PR; final cross-platform validation/review pending |

The public repository listing proves repository creation/forking, not merge or release readiness. Both source PRs and the index PR remain drafts. The latest index validation/platform workspace jobs passed, but its openkal-compat measurement job was still running at this audit; the whole PR cannot be described as green yet. ORT CPU acceptance is independent of OpenCV full support.

## Live work and remaining gates

- Monitor current index and llama PR heads, not the earlier failing runs. Index Windows UTF-8 decoding and current shared-image adapter metadata were corrected. Llama Vulkan CI pins the candidate index so it can validate those fixes before merge; native ARM verifies managed ELF runtime closure rather than assuming a static file label. No strict checks were disabled.
- OpenCV source integration is still 8 of 16 main modules. Import/build/export/behavior auditing for the remaining eight is required.
- Finish native highgui Win32/Cocoa validation, video native backends and the DNN ONNX Runtime bridge with concrete features/templates/actual runtime tests. The highgui feature alone remains API-only; native backends require explicit selection. Existing DNN tests do not prove ORT execution.
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
