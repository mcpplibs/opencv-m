#pragma once
// Freeze ordinary SDK declarations before adjusting namespace-function linkage.
#include <opencv2/core.hpp>
#if defined(MCPP_FEATURE_SIMD_EMULATED)
#define CV_FORCE_SIMD128_CPP 1
// The emulator uses CV_INLINE only on namespace function templates. Give those
// original templates external linkage so named modules can re-export them.
// Vector class definitions and all implementation bodies remain SDK originals.
#pragma push_macro("CV_INLINE")
#undef CV_INLINE
#define CV_INLINE inline
#endif
#include <opencv2/core/simd_intrinsics.hpp>
#if defined(MCPP_FEATURE_SIMD_EMULATED)
#pragma pop_macro("CV_INLINE")
#endif
