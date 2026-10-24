#include <gtest/gtest.h>
#include <array>
#include <cmath>

import mcpplibs.mwrap.opencv.core;
import mcpplibs.mwrap.opencv.macros;
#if defined(MCPP_FEATURE_IMGPROC)
import mcpplibs.mwrap.opencv.imgproc;
#endif

TEST(HalInterface, CoreArithmeticDispatchRetainsSaturationAndNorms) {
    const std::array<uchar, 4> first{0, 255, 10, 250}, second{1, 1, 20, 20};
    std::array<uchar, 4> result{};
    cv::hal::add8u(first.data(), 4, second.data(), 4, result.data(), 4, 4, 1, nullptr);
    EXPECT_EQ(result, (std::array<uchar, 4>{1, 255, 30, 255}));
    EXPECT_EQ(cv::hal::normHamming(first.data(), 4), 16);
    EXPECT_EQ(cv::hal::normL1_(first.data(), second.data(), 4), 495);
    const std::array<float, 3> input{0, 1, -1};
    std::array<float, 3> exponent{};
    cv::hal::exp32f(input.data(), exponent.data(), 3);
    EXPECT_NEAR(exponent[0], 1, 1e-6);
    EXPECT_NEAR(exponent[1], std::exp(1.f), 1e-5);
    EXPECT_NEAR(exponent[2], std::exp(-1.f), 1e-6);
}

#if defined(MCPP_FEATURE_IMGPROC)
TEST(HalInterface, ColorConversionMatchesThePublicImageApi) {
    namespace types = mcpplibs::mwrap::opencv::macros;
    cv::Mat bgr(1, 3, types::CV_8UC3);
    bgr.at<cv::Vec3b>(0, 0) = cv::Vec3b(20, 80, 160);
    bgr.at<cv::Vec3b>(0, 1) = cv::Vec3b(255, 0, 0);
    bgr.at<cv::Vec3b>(0, 2) = cv::Vec3b(0, 255, 0);
    cv::Mat gray(1, 3, types::CV_8UC1), reference;
    cv::hal::cvtBGRtoGray(bgr.data, bgr.step[0], gray.data, gray.step[0],
                         bgr.cols, bgr.rows, types::CV_8U, 3, false);
    cv::cvtColor(bgr, reference, cv::COLOR_BGR2GRAY);
    EXPECT_EQ(cv::norm(gray, reference, cv::NORM_INF), 0);
    cv::Mat expanded(1, 3, types::CV_8UC3);
    cv::hal::cvtGraytoBGR(gray.data, gray.step[0], expanded.data, expanded.step[0],
                         gray.cols, gray.rows, types::CV_8U, 3);
    for (int col = 0; col < gray.cols; ++col) {
        const auto value = gray.at<uchar>(0, col);
        EXPECT_EQ(expanded.at<cv::Vec3b>(0, col), cv::Vec3b(value, value, value));
    }
}
#endif
