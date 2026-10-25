#pragma once
#include <filesystem>
#include <string>

// ORT's profiling path is wchar_t on Windows, while OpenCV stores the
// temporary filename as std::string. Adapt only the implementation call.
namespace mwrap_opencv_port {
inline auto ort_profile_path(const std::string& value) {
    return std::filesystem::path(value).native();
}
}
