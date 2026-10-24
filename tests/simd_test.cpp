#include <gtest/gtest.h>
#if defined(MCPP_FEATURE_SIMD)
#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
import mcpplibs.mwrap.opencv.core.simd;
import mcpplibs.mwrap.opencv.macros;
#include "fixtures/simd_fixture.hpp"
TEST(Simd, OriginalSdkArithmeticMemoryPackingAndLaneContracts) {
    EXPECT_NO_THROW(simd_fixture::emit("module"));
}
TEST(Simd, TypedCapabilitiesDescribeTheCompiledLibraryBaseline) {
    namespace macros = mcpplibs::mwrap::opencv::macros;
    static_assert(macros::CV_SIMD_WIDTH == 16);
    static_assert(macros::CV_SIMD128 == 1 && macros::CV_SIMD128_64F == 1);
    static_assert(macros::CV_SIMD256 == 0 && macros::CV_SIMD512 == 0);
    EXPECT_EQ(sizeof(cv::v_float32), macros::CV_SIMD_WIDTH);
    EXPECT_EQ(cv::VTraits<cv::v_float32>::vlanes(), 4);
}
#endif
