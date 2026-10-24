// C++ module adapter; upstream cv:: names remain unchanged.
module;
#include <opencv2/ptcloud.hpp>
#include <opencv2/ptcloud/detail/kinfu_frame.hpp>

export module mcpplibs.mwrap.opencv.detail.ptcloud;

#include "../gen_exports/ptcloud.inc"
