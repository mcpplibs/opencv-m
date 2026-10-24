#include <gtest/gtest.h>
#include <type_traits>
#include <vector>

import mcpplibs.mwrap.opencv.core;
import mcpplibs.mwrap.opencv.macros;

TEST(CoreCppTypes, UpstreamGlobalAndCppAliasesRemainAvailable) {
    static_assert(std::is_same_v<uchar, unsigned char>);
    static_assert(std::is_same_v<schar, signed char>);
    static_assert(std::is_same_v<ushort, unsigned short>);
    static_assert(sizeof(int64) == 8 && sizeof(uint64) == 8);
    static_assert(std::is_same_v<cv::int64_t, std::int64_t>);
    static_assert(sizeof(cv::Vec3l::value_type) == 8);
    static_assert(sizeof(cv::bfloat) == 2 && sizeof(cv::hfloat) == 2);
    static_assert(CPU_AVX512_ICL > CPU_AVX2 && CPU_RVV_ZVFH > CPU_RVV);
    Cv32suf bits{};
    bits.f = 1.0f;
    EXPECT_EQ(bits.u, 0x3f800000u);
    cv::MatND matrix(2, 3, mcpplibs::mwrap::opencv::macros::CV_8UC1);
    EXPECT_EQ(matrix.total(), 6u);
}

TEST(CoreCppTypes, AffineCompositionPointsVectorsAndInverse) {
    const cv::Affine3d translation(cv::Vec3d(0, 0, 0), cv::Vec3d(1, 2, 3));
    const auto vector = translation * cv::Vec3d(4, 5, 6);
    const auto point = translation * cv::Point3d(4, 5, 6);
    EXPECT_DOUBLE_EQ(vector[0], 5);
    EXPECT_DOUBLE_EQ(vector[1], 7);
    EXPECT_DOUBLE_EQ(vector[2], 9);
    EXPECT_DOUBLE_EQ(point.x, 5);
    EXPECT_DOUBLE_EQ(point.y, 7);
    EXPECT_DOUBLE_EQ(point.z, 9);
    const auto identity = translation.inv() * translation;
    const auto restored = identity * cv::Vec3d(4, 5, 6);
    EXPECT_NEAR(restored[0], 4, 1e-12);
    EXPECT_NEAR(restored[1], 5, 1e-12);
    EXPECT_NEAR(restored[2], 6, 1e-12);
}

TEST(CoreCppTypes, QuaternionRotationAndGrowingAutoBuffer) {
    const auto q = cv::Quatd::createFromAngleAxis(
        mcpplibs::mwrap::opencv::macros::CV_PI / 2, cv::Vec3d(0, 0, 1));
    const auto rotated = q.toRotMat3x3() * cv::Vec3d(1, 0, 0);
    EXPECT_NEAR(rotated[0], 0, 1e-12);
    EXPECT_NEAR(rotated[1], 1, 1e-12);
    EXPECT_NEAR(rotated[2], 0, 1e-12);
    cv::AutoBuffer<int, 4> buffer(3, 7);
    buffer.resize(12);
    EXPECT_EQ(buffer.size(), 12u);
    EXPECT_EQ(buffer.data()[2], 7);
    buffer.data()[11] = 42;
    EXPECT_EQ(buffer.data()[11], 42);
    std::vector<int> labels;
    const int classes = cv::partition(std::vector<int>{1, 2, 1, 3, 2}, labels,
        [](int a, int b) { return a == b; });
    EXPECT_EQ(classes, 3);
    EXPECT_EQ(labels[0], labels[2]);
    EXPECT_EQ(labels[1], labels[4]);
    EXPECT_NE(labels[0], labels[1]);
}

TEST(CoreCppTypes, ErrorCodesTraitsAndLoggingKeepTheirCppSurface) {
    static_assert(cv::traits::Type<cv::Vec3b>::value == mcpplibs::mwrap::opencv::macros::CV_8UC3);
    static_assert(cv::traits::Depth<float>::value == mcpplibs::mwrap::opencv::macros::CV_32F);
    static_assert(cv::ParamType<int>::type == cv::Param::INT);
    try {
        cv::error(cv::Error::StsBadArg, "module error surface", __func__, __FILE__, __LINE__);
        FAIL() << "cv::error must throw";
    } catch (const cv::Exception& error) {
        EXPECT_EQ(error.code, cv::Error::StsBadArg);
    }
    namespace logging = cv::utils::logging;
    const auto previous = logging::setLogLevel(logging::LOG_LEVEL_DEBUG);
    EXPECT_EQ(logging::getLogLevel(), logging::LOG_LEVEL_DEBUG);
    logging::setLogLevel(previous);
}

TEST(CoreCppTypes, QuaternionFreeFunctionsOperateThroughDirectImport) {
    const auto quarter_turn = cv::Quatd::createFromAngleAxis(
        mcpplibs::mwrap::opencv::macros::CV_PI / 2, cv::Vec3d(0, 0, 1));
    const auto inverted = cv::inv(quarter_turn).toRotMat3x3() * cv::Vec3d(1, 0, 0);
    EXPECT_NEAR(inverted[0], 0, 1e-12);
    EXPECT_NEAR(inverted[1], -1, 1e-12);
    const auto half_turn = cv::power(quarter_turn, 2.0).toRotMat3x3() * cv::Vec3d(1, 0, 0);
    EXPECT_NEAR(half_turn[0], -1, 1e-12);
    EXPECT_NEAR(half_turn[1], 0, 1e-12);
}
