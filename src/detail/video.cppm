// C++ module adapter; upstream cv:: names remain unchanged.
module;
#include <opencv2/video.hpp>
#include <opencv2/video/detail/tracking.detail.hpp>

export module mcpplibs.mwrap.opencv.detail.video;

#include "../gen_exports/video.inc"
