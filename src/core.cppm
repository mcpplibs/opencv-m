// opencv.core — C++23 module over OpenCV 5 core. The C++ API is unchanged:
// every exported name is the upstream entity itself (`export using`), only
// the header inclusion goes away. Function-like macros (CV_Assert, CV_Error,
// …) cannot cross a module boundary — include <mwrap/opencv/macros.hpp> BEFORE
// importing when you need them.
module;
#include <cstdarg>   // va_list/va_start/va_copy/va_end — cv::format printf reimpl (core_fns.inc)
#include <cstdio>    // vsnprintf/fputs/FILE/stdout — cv::format / cv::print reimpls (core_fns.inc)
#include <opencv2/core.hpp>
#include <opencv2/core/async.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/quaternion.hpp>
#include <opencv2/core/softfloat.hpp>
#include <opencv2/core/version.hpp>

export module mcpplibs.mwrap.opencv.core;

#include "gen_exports/core.inc"
#include "core_ops.inc"
#include "matx_ops.inc"
#include "core_fns.inc"

// ── global-scope helpers (outside namespace cv upstream) ────────────────
export using ::CpuFeatures;
export using ::CPU_MMX;
export using ::CPU_SSE;
export using ::CPU_SSE2;
export using ::CPU_SSE3;
export using ::CPU_SSSE3;
export using ::CPU_SSE4_1;
export using ::CPU_SSE4_2;
export using ::CPU_POPCNT;
export using ::CPU_FP16;
export using ::CPU_AVX;
export using ::CPU_AVX2;
export using ::CPU_FMA3;
export using ::CPU_AVX512_SKX;
export using ::CPU_NEON;
export using ::CPU_MAX_FEATURE;
