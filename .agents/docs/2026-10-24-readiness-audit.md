# Implementation and release readiness ledger

The full objective remains the OpenCV 5/main C++ API and backend plan, ORT wrapper, executable -m templates, one checkpoint PR per repository, green relevant CI and ecosystem review. Nothing is merged or released by these checkpoints. mcpp engine code stays read-only; its existing documentation-only PR is not a prerequisite.

## Verified current checkpoints

- OpenCV head 004b4a4: native CI run 40022631743 passed all five jobs, including core, image codec behavior and the real default project template; Linux GCC additionally runs DNN/unifont. Root LICENSE/metadata/readme now consistently say Apache-2.0. The image example explicitly requests required features.
- ORT head de3a0a9: native CI run 40022727603 passed Linux GCC/LLVM, macOS arm64 and Windows MSVC ABI. Each runs three module consumers plus the actual project template. LLVM audits the exact SDK declaration inventory. Supplemental official headers omitted from Windows SDK are source/hash pinned. The config-module internal-linkage adaptation and mixing limits are explicit.
- SDK provider: isolated SDK install plus LLVM/GCC inference/header tests passed; no wrapper/source cache was preseeded in that SDK test.
- ORT immutable archive: source de3a0a9da143f4ae52eb5e1a0302ba3fdf623b6d downloaded twice with SHA256 24d4bc26d2f426e3643f21d289d92e7a23b3ba54524a099a1079c1c490915251. Index consumers execute real inference and public helper/config APIs. The complete unchanged publication layout also passed in a fresh MCPP_HOME: actual archive/SDK/toolchain installation, mcpp new, strict build and unmodified template run.
- Index PR #564 contains the ORT wrapper descriptor, SDK provider, mwrap specification/skill and category/test discovery changes. The full updater audit passed (85 automatic declarations, one manual immutable snapshot), and all 14 updater adapter tests passed. Namespace-filtered views are test-only byte copies; normal publication layout is tested separately and is never converted.
- Llama PR #5 adds b10069.3 CPU template, keeping its legacy package/module identity. No-argument startup and a checksummed GGUF decode/sample passed through actual mcpp new consumers. Root LLVM build and all 116 existing unit/repository/build-helper checks passed. Native CI additionally exercises the template; compatibility corrections are still being verified.

## Live work and remaining gates

- Monitor current index and llama PR heads, not the earlier failing runs. Index Windows UTF-8 decoding and current shared-image adapter metadata were corrected. Llama Vulkan CI pins the candidate index so it can validate those fixes before merge; native ARM verifies managed ELF runtime closure rather than assuming a static file label. No strict checks were disabled.
- OpenCV source integration is still 8 of 16 main modules. Import/build/export/behavior auditing for the remaining eight is required.
- Native highgui GTK3/Win32/Cocoa, video native backends, DNN ONNX Runtime bridge and their concrete features/templates/actual runtime tests remain required work. The current highgui is API-only. Existing DNN tests do not prove ORT execution.
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
