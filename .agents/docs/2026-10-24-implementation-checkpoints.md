# OpenCV mwrap implementation checkpoints

## Source/interface separation checkpoint

- Fork exists at mcpplibs/opencv-m; implementation branch retains commits.
- Existing pinned upstream source moved to upstream/opencv without content changes.
- Generated snapshots moved to generated; compatibility header moved to port/include/mwrap/opencv.
- Root and submodule names changed to mcpplibs.mwrap.opencv; upstream cv API preserved.
- Macro constants moved from cv into mcpplibs::mwrap::opencv::macros.
- LLVM 23.1.3 baseline build passed (2747 units including the existing FFmpeg closure).
- Existing test suite: six binaries passed; DNN/unifont gated cases require their feature runs separately.
- Package revision 5.0.0.1 distinguishes this wrapper from the inherited v5.0.0 tag without changing upstream version.
- Remaining: source/feature catalog split, additional modules, native GUI, ORT bridge, codecs/backends, full API audit, native CI and installed/template consumers.

## Optional source catalog checkpoint

- Generated per-platform source catalogs separate mandatory core, optional modules, codecs and video backends.
- The root module aggregates only selected module interfaces; FFmpeg is a videoio-ffmpeg feature dependency.
- Build-instance configuration and build information reflect the chosen JPEG/PNG features.
- Linux LLVM 23.1.3 strict core-only and PNG/JPEG configurations passed all six test binaries. The image configuration executed both actual codec roundtrips; feature-disabled DNN/unifont binaries contain no cases and do not establish coverage for those capabilities.
- CI now runs image codec features separately from core-only and DNN/unifont configurations.
- mcpp engine is read-only. Its existing documentation/skill PR remains unmerged and is not a dependency of this work.
- Native GUI, remaining main modules and ORT bridge remain implementation work; this checkpoint does not claim full support.

## Aggregate module CI correction

- Run 40010892459 exposed imports introduced through a header: GCC rejected them, while Windows built the library but consumers could not see the aggregate's core API.
- The conventional src/opencv.cppm now directly re-exports core, macros and an internal selection module.
- build.mcpp writes that selection module into its configuration-specific output and declares it with mcpp::generated before scanning. The generated file contains concrete imports for the selected features, with no header-induced or preprocessor-conditional imports.
- Existing root-import API, header-mixing and codec tests exercise the aggregate from consumers; native Windows verification remains in CI.
- This correction requires no mcpp engine changes.
