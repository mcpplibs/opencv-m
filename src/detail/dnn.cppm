// opencv.dnn — C++23 module over OpenCV 5's deep-learning module (API
// unchanged). OPTIONAL: this file is compiled only when the package `dnn`
// feature is enabled, which also builds the vendored dnn module sources
// (+ vendored protobuf + mlas) into the library. It is
// selected explicitly or by all-modules — consumers can import it directly with
// `import mcpplibs.mwrap.opencv.dnn;` (Net, blobFromImage, readNet, NMSBoxes, …).
module;
#include <opencv2/dnn.hpp>
// <opencv2/dnn.hpp> pulls in only the core dnn surface (Net, blobFromImage,
// readNet, Model classes, Backend/Target). The 181 Layer types + ACTIV_/
// AUTO_PAD_ enums live in this separate public header — include it so they are
// visible to the `export using` list below.
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/dnn/shape_utils.hpp>

export module mcpplibs.mwrap.opencv.detail.dnn;

#include "../gen_exports/dnn.inc"

export namespace cv::dnn {
CV__DNN_INLINE_NS_BEGIN
inline namespace opencv_m_shape {
template<class> inline constexpr bool shape_pick = true;
#include "../dnn_shapes.inc"
}
CV__DNN_INLINE_NS_END
using CV__DNN_INLINE_NS::opencv_m_shape::slice;
using CV__DNN_INLINE_NS::opencv_m_shape::getPlane;
using CV__DNN_INLINE_NS::opencv_m_shape::shape;
using CV__DNN_INLINE_NS::opencv_m_shape::total;
using CV__DNN_INLINE_NS::opencv_m_shape::concat;
using CV__DNN_INLINE_NS::opencv_m_shape::toString;
using CV__DNN_INLINE_NS::opencv_m_shape::print;
using CV__DNN_INLINE_NS::opencv_m_shape::operator<<;
using CV__DNN_INLINE_NS::opencv_m_shape::normalize_axis;
using CV__DNN_INLINE_NS::opencv_m_shape::normalize_axis_range;
using CV__DNN_INLINE_NS::opencv_m_shape::isAllOnes;
}
