#include <gtest/gtest.h>
#include <limits>
#include <type_traits>
#include <tuple>
#include <opencv2/core/saturate.hpp>

// Bind the upstream header overloads before importing the adapter namespace.
namespace upstream_saturate {
template<class To, class From>
inline constexpr auto function = static_cast<To (*)(From)>(&cv::saturate_cast<To>);
const auto unsigned_from_int = function<unsigned, int>;
const auto byte_from_short = function<uchar, ushort>;
const auto wide_from_double = function<uint64, double>;
}

import mcpplibs.mwrap.opencv.core;

TEST(SaturateParity, NegativeIntegersClampForUnsignedDestinations) {
    ASSERT_EQ(upstream_saturate::unsigned_from_int(-1), 0u);
    EXPECT_EQ(cv::opencv_m_repl::saturate_cast<unsigned>(-1), upstream_saturate::unsigned_from_int(-1));
}

TEST(SaturateParity, WideUnsignedSourcesClampForByteDestinations) {
    ASSERT_EQ(upstream_saturate::byte_from_short(256), 255);
    EXPECT_EQ(cv::opencv_m_repl::saturate_cast<uchar>(ushort(256)), upstream_saturate::byte_from_short(256));
}

TEST(SaturateParity, WideIntegerDestinationsFollowUpstreamRounding) {
    ASSERT_EQ(upstream_saturate::wide_from_double(1.8), 2u);
    EXPECT_EQ(cv::opencv_m_repl::saturate_cast<uint64>(1.8), upstream_saturate::wide_from_double(1.8));
}

namespace {
using ScalarTypes = std::tuple<uchar, schar, ushort, short, unsigned, int, int64, uint64,
                               float, double, cv::hfloat, cv::bfloat, bool>;
template<class To, class From>
void check_conversion(From value) {
    const auto expected = upstream_saturate::function<To, From>(value);
    const auto actual = cv::opencv_m_repl::saturate_cast<To>(value);
    if constexpr (std::is_same_v<To, cv::hfloat> || std::is_same_v<To, cv::bfloat>)
        EXPECT_EQ(float(actual), float(expected));
    else
        EXPECT_EQ(actual, expected);
}
template<class From, std::size_t... Index>
void check_destinations(From value, std::index_sequence<Index...>) {
    (check_conversion<std::tuple_element_t<Index, ScalarTypes>>(value), ...);
}
template<class From>
void check_values(std::initializer_list<From> values) {
    for (const auto value : values)
        check_destinations(value, std::make_index_sequence<std::tuple_size_v<ScalarTypes>>{});
}
}

TEST(SaturateParity, AllThirteenScalarTypesMatchThePinnedHeader) {
    check_values<uchar>({0, 1, 128, 255});
    check_values<schar>({-128, -1, 0, 127});
    check_values<ushort>({0, 255, 256, 65535});
    check_values<short>({-32768, -1, 256, 32767});
    check_values<unsigned>({0, 256, 65536, std::numeric_limits<unsigned>::max()});
    check_values<int>({std::numeric_limits<int>::min(), -1, 0, 128, 32768, std::numeric_limits<int>::max()});
    check_values<int64>({std::numeric_limits<int64>::min(), -1, 0, 256, std::numeric_limits<int64>::max()});
    check_values<uint64>({0, 256, 65536, std::numeric_limits<unsigned>::max(), std::numeric_limits<uint64>::max()});
    check_values<float>({-32768.5f, -2.5f, -0.1f, 0, 1.8f, 2.5f, 3.5f, 255.5f, 32767.5f, 65504});
    check_values<double>({-32768.5, -2.5, -0.1, 0, 1.8, 2.5, 3.5, 255.5, 32767.5, 65504});
    check_values<cv::hfloat>({cv::hfloat(-2.5f), cv::hfloat(0.f), cv::hfloat(1.8f), cv::hfloat(65504.f)});
    check_values<cv::bfloat>({cv::bfloat(-2.5f), cv::bfloat(0.f), cv::bfloat(1.8f), cv::bfloat(65504.f)});
    check_values<bool>({false, true});
}
