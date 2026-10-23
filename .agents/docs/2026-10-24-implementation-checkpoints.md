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
