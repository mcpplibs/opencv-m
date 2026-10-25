// Preserve both upstream setFlags overloads without exporting the SDK's
// internal-linkage int overload. All state remains owned by the SDK.
namespace opencv_m_instrumentation_up {
inline constexpr auto set_flags = static_cast<void (*)(cv::instr::FLAGS)>(&cv::instr::setFlags);
}
export namespace cv::instr {
using ::cv::instr::operator==;
inline namespace opencv_m_repl {
template<class _D = void> requires ::cv::opencv_m_repl::opencv_m_pick<_D>
inline void setFlags(FLAGS flags) { ::opencv_m_instrumentation_up::set_flags(flags); }
template<class _D = void> requires ::cv::opencv_m_repl::opencv_m_pick<_D>
inline void setFlags(int flags) { ::opencv_m_instrumentation_up::set_flags(static_cast<FLAGS>(flags)); }
} }
