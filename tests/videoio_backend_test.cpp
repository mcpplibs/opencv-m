#include <gtest/gtest.h>
#if defined(MCPP_FEATURE_VIDEOIO_NATIVE) || defined(MCPP_FEATURE_VIDEOIO_V4L2) || defined(MCPP_FEATURE_VIDEOIO_MSMF) || defined(MCPP_FEATURE_VIDEOIO_AVFOUNDATION) || defined(MCPP_FEATURE_VIDEOIO_FFMPEG)
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>

import mcpplibs.mwrap.opencv;

namespace {
void roundtrip_video(int backend, int codec, const char* extension) {
    struct VideoFile {
        std::filesystem::path path;
        ~VideoFile() { std::error_code error; std::filesystem::remove(path, error); }
    } file{std::filesystem::temp_directory_path() / ("mwrap-video-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + extension)};
    const auto expected_backend = cv::videoio_registry::getBackendName(static_cast<cv::VideoCaptureAPIs>(backend));
    cv::VideoWriter writer(file.path.string(), backend, codec, 24.0, cv::Size(96, 64));
    ASSERT_TRUE(writer.isOpened()) << expected_backend;
    ASSERT_EQ(writer.getBackendName(), expected_backend);
    for (int i = 0; i < 12; ++i) {
        cv::Mat image(64, 96, mcpplibs::mwrap::opencv::macros::CV_8UC3, cv::Scalar(30 + i, 60, 90));
        writer.write(image);
    }
    writer.release();
    ASSERT_GT(std::filesystem::file_size(file.path), 0u);
    cv::VideoCapture reader(file.path.string(), backend);
    ASSERT_TRUE(reader.isOpened()) << expected_backend;
    ASSERT_EQ(reader.getBackendName(), expected_backend);
    cv::Mat decoded;
    int count = 0;
    while (reader.read(decoded)) {
        ASSERT_FALSE(decoded.empty());
        ASSERT_TRUE(decoded.size() == cv::Size(96, 64));
        ASSERT_EQ(decoded.channels(), 3);
        cv::Mat reference(64, 96, mcpplibs::mwrap::opencv::macros::CV_8UC3, cv::Scalar(30 + count, 60, 90));
        EXPECT_LT(cv::norm(decoded, reference, cv::NORM_L1) / (64.0 * 96 * 3), 15.0);
        ++count;
    }
    EXPECT_EQ(count, 12);
}
}

#if defined(MCPP_FEATURE_VIDEOIO_NATIVE) || defined(MCPP_FEATURE_VIDEOIO_V4L2) || defined(MCPP_FEATURE_VIDEOIO_MSMF) || defined(MCPP_FEATURE_VIDEOIO_AVFOUNDATION)
TEST(VideoioBackend, NativeBackendIsBuiltInAndRegistered) {
#if defined(__linux__)
    const auto backend = cv::CAP_V4L2;
#elif defined(_WIN32)
    const auto backend = cv::CAP_MSMF;
#else
    const auto backend = cv::CAP_AVFOUNDATION;
#endif
    EXPECT_TRUE(cv::videoio_registry::hasBackend(backend));
    EXPECT_TRUE(cv::videoio_registry::isBackendBuiltIn(backend));
    const auto cameras = cv::videoio_registry::getCameraBackends();
    EXPECT_NE(std::find(cameras.begin(), cameras.end(), backend), cameras.end());
}

#if defined(__linux__)
TEST(VideoioBackend, V4l2ReportsAnUnavailableDevice) {
    cv::VideoCapture absent(99999, cv::CAP_V4L2);
    EXPECT_FALSE(absent.isOpened()); // No physical-camera coverage is claimed.
}
#else
TEST(VideoioBackend, NativeFileEncodingAndDecoding) {
#if defined(_WIN32)
    const int backend = cv::CAP_MSMF;
#else
    const int backend = cv::CAP_AVFOUNDATION;
#endif
    roundtrip_video(backend, cv::VideoWriter::fourcc('H', '2', '6', '4'), ".mp4");
}
#endif
#endif

#if defined(MCPP_FEATURE_VIDEOIO_FFMPEG)
TEST(VideoioBackend, FfmpegFileEncodingAndDecoding) {
    roundtrip_video(cv::CAP_FFMPEG, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), ".avi");
}
#endif
#endif
