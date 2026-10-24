// C++ module adapter; upstream cv:: names remain unchanged.
module;
#include <opencv2/photo.hpp>
#include <opencv2/photo/cuda.hpp>

export module mcpplibs.mwrap.opencv.detail.photo;

#include "../gen_exports/photo.inc"
