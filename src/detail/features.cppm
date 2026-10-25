// C++ module adapter; upstream cv:: names remain unchanged.
module;
#include <opencv2/features.hpp>
#include <opencv2/features/hal/interface.h>

export module mcpplibs.mwrap.opencv.detail.features;

#include "../gen_exports/features.inc"
