#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <vector>

#if defined(MCPP_FEATURE_OBJDETECT)
import mcpplibs.mwrap.opencv.objdetect;
TEST(ImageHelpers, ArucoThresholdMatchesTheSdkDetectorDefault) {
    static_assert(cv::aruco::DEFAULT_VALID_BIT_ID_THRESHOLD == 0.49f);
    const cv::aruco::DetectorParameters defaults;
    EXPECT_FLOAT_EQ(defaults.validBitIdThreshold, cv::aruco::DEFAULT_VALID_BIT_ID_THRESHOLD);
    const float* volatile address = &cv::aruco::DEFAULT_VALID_BIT_ID_THRESHOLD;
    EXPECT_FLOAT_EQ(*address, 0.49f);
}
#endif

#if defined(MCPP_FEATURE_IMGPROC)
import mcpplibs.mwrap.opencv.imgproc;
import mcpplibs.mwrap.opencv.macros;
namespace types = mcpplibs::mwrap::opencv::macros;

TEST(ImageHelpers, ExplicitMagicBorderMatchesDefaultErosionAndDilation) {
    cv::Mat image(2, 2, types::CV_64F, cv::Scalar(7)), implicit, explicit_border;
    cv::Mat kernel = cv::Mat::ones(3, 3, types::CV_8U);
    const auto border = cv::morphologyDefaultBorderValue();
    cv::erode(image, implicit, kernel);
    cv::erode(image, explicit_border, kernel, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, border);
    EXPECT_DOUBLE_EQ(cv::norm(implicit, explicit_border, cv::NORM_INF), 0);
    EXPECT_DOUBLE_EQ(explicit_border.at<double>(0, 0), 7);
    cv::dilate(image, implicit, kernel);
    cv::dilate(image, explicit_border, kernel, cv::Point(-1, -1), 1, cv::BORDER_CONSTANT, border);
    EXPECT_DOUBLE_EQ(cv::norm(implicit, explicit_border, cv::NORM_INF), 0);
    EXPECT_DOUBLE_EQ(explicit_border.at<double>(0, 0), 7);
}

TEST(ImageHelpers, WideAngleDefaultEnumAndIntegerOverloadsAgree) {
    cv::Mat camera = cv::Mat::eye(3, 3, types::CV_64F);
    camera.at<double>(0, 0) = camera.at<double>(1, 1) = 30;
    camera.at<double>(0, 2) = 16;
    camera.at<double>(1, 2) = 12;
    cv::Mat distortion = cv::Mat::zeros(5, 1, types::CV_64F);
    cv::Mat default_x, default_y, enum_x, enum_y, integer_x, integer_y;
    const float baseline = cv::initWideAngleProjMap(camera, distortion, cv::Size(32, 24), 32,
        types::CV_32FC1, default_x, default_y);
    const float explicit_enum = cv::initWideAngleProjMap(camera, distortion, cv::Size(32, 24), 32,
        types::CV_32FC1, enum_x, enum_y, cv::PROJ_SPHERICAL_EQRECT, 0);
    const float integer = cv::initWideAngleProjMap(camera, distortion, cv::Size(32, 24), 32,
        types::CV_32FC1, integer_x, integer_y, static_cast<int>(cv::PROJ_SPHERICAL_EQRECT));
    EXPECT_TRUE(std::isfinite(baseline));
    EXPECT_GT(baseline, 0);
    EXPECT_EQ(default_x.cols, 32);
    EXPECT_FLOAT_EQ(baseline, explicit_enum);
    EXPECT_FLOAT_EQ(baseline, integer);
    EXPECT_DOUBLE_EQ(cv::norm(default_x, enum_x, cv::NORM_INF), 0);
    EXPECT_DOUBLE_EQ(cv::norm(default_y, enum_y, cv::NORM_INF), 0);
    EXPECT_DOUBLE_EQ(cv::norm(default_x, integer_x, cv::NORM_INF), 0);
    EXPECT_DOUBLE_EQ(cv::norm(default_y, integer_y, cv::NORM_INF), 0);
}
#endif

#if defined(MCPP_FEATURE_IMGCODECS)
import mcpplibs.mwrap.opencv.imgcodecs;

TEST(ImageHelpers, ImwriteMultiRetainsTheSingleFrameSdkDispatch) {
    struct Cleanup {
        cv::String path = cv::tempfile(".bmp");
        ~Cleanup() { std::error_code error; std::filesystem::remove(path, error); }
    } file;
    cv::Mat image(8, 8, types::CV_8UC3, cv::Scalar(20, 80, 160));
    const std::vector<cv::Mat> images{image};
    ASSERT_TRUE(cv::imwritemulti(file.path, images));
    const auto restored = cv::imread(file.path, cv::IMREAD_UNCHANGED);
    ASSERT_FALSE(restored.empty());
    EXPECT_DOUBLE_EQ(cv::norm(restored, image, cv::NORM_INF), 0);
}
#endif
