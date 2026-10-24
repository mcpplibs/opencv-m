// opencv.core — C++23 module over OpenCV 5 core. The C++ API is unchanged:
// external API entities retain their SDK identity through `export using`.
// TU-local helpers use SDK-body adapters or exact platform function forwards.
// Function-like macros (CV_Assert, CV_Error,
// …) cannot cross a module boundary — include <mwrap/opencv/macros.hpp> BEFORE
// importing when you need them.
module;
#include <cstdarg>   // va_list/va_start/va_copy/va_end — cv::format printf reimpl (core_fns.inc)
#include <cstdio>    // vsnprintf/fputs/FILE/stdout — cv::format / cv::print reimpls (core_fns.inc)
#include <sstream>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/hal.hpp>
#include <opencv2/core/llapi/llapi.h>
#include <opencv2/core/affine.hpp>
#include <opencv2/core/async.hpp>
#include <opencv2/core/bindings_utils.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/core/detail/async_promise.hpp>
#include <opencv2/core/dualquaternion.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/core/parallel/parallel_backend.hpp>
#include <opencv2/core/quaternion.hpp>
#include <opencv2/core/softfloat.hpp>
#include <opencv2/core/utils/allocator_stats.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/core/utils/fp_control_utils.hpp>
#include <opencv2/core/utils/instrumentation.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/core/utils/tls.hpp>
#include <opencv2/core/version.hpp>

export module mcpplibs.mwrap.opencv.detail.core;

#include "../gen_exports/core.inc"
#include "../core_ops.inc"
#include "../type_ops.inc"
#include "../core_fns.inc"
#include "../portable_helpers.inc"
#include "../fast_math_helpers.inc"
#include "../cuda_helpers.inc"
#include "../utils_helpers.inc"
#include "../persistence_ops.inc"
#include "../instrumentation_ops.inc"
#include "../affine_ops.inc"
