// Original OpenCV intrinsics for the selected library baseline; no SDK type copies.
module;
#include <opencv2/core/simd_intrinsics.hpp>

#if !CV_SSE2 && !(CV_NEON && (defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)))
#error "opencv-m simd currently requires an x86_64 SSE2 or ARM64 NEON baseline"
#endif

export module mcpplibs.mwrap.opencv.detail.simd;
#include "../simd_exports.inc"
