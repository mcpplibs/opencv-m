// mwrap/opencv/macros.hpp — optional side header for OpenCV's macro surface.
//
// Modules cannot export macros. Everything with linkage is re-exported by the
// opencv.* modules; this textual header covers the rest: CV_Assert / CV_Error
// / CV_DbgAssert, the CV_8UC(n)-style *function-like* type macros, version
// macros, and the global-namespace inline helpers (cvRound, cvFloor, …).
//
// IMPORTANT: include this header BEFORE any `import mcpplibs.mwrap.opencv.*;` in the same
// translation unit (include-after-import is ill-formed for headers that also
// declare entities attached to the global module — same rule as ffmpeg-m's
// macros.h).
#pragma once

#include <opencv2/core/base.hpp>
#include <opencv2/core/check.hpp>
#include <opencv2/core/cvdef.h>
#include <opencv2/core/version.hpp>
#include <opencv2/features/hal/interface.h>
#include <opencv2/imgproc/hal/interface.h>

// OpenCV 5.0.0 spells the flag CV_MAT_SUBMAT_FLAG in this macro, but only
// CV_SUBMAT_FLAG exists. Keep the vendor file intact and repair the optional
// compatibility surface, matching Mat::isSubmatrix and the typed mapping.
#if CV_VERSION_MAJOR == 5 && CV_VERSION_MINOR == 0 && CV_VERSION_REVISION == 0
#undef CV_IS_SUBMAT
#define CV_IS_SUBMAT(flags) ((flags) & CV_SUBMAT_FLAG)
#endif
