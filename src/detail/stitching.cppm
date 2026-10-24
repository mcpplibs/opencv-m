// C++ module adapter; upstream cv:: names remain unchanged.
module;
#include <opencv2/stitching.hpp>
#include <opencv2/stitching/detail/autocalib.hpp>
#include <opencv2/stitching/detail/timelapsers.hpp>

export module mcpplibs.mwrap.opencv.detail.stitching;

#include "../gen_exports/stitching.inc"
