// Capture the SDK's TU-local constant by value; its using-declaration cannot
// cross a module boundary. The public qualified name and value are preserved.
namespace opencv_m_aruco_up {
inline constexpr auto valid_bit_id_threshold = ::cv::aruco::DEFAULT_VALID_BIT_ID_THRESHOLD;
}
export namespace cv::aruco { inline namespace opencv_m_constants {
inline constexpr auto DEFAULT_VALID_BIT_ID_THRESHOLD = ::opencv_m_aruco_up::valid_bit_id_threshold;
} }
