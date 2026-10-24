#include <gtest/gtest.h>
#if defined(MCPP_FEATURE_SIMD)
#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
import mcpplibs.mwrap.opencv.core.simd.legacy;
#include "fixtures/simd_legacy_fixture.hpp"
TEST(SimdLegacy, OriginalOperatorsReferencesAndSaturation) {
    EXPECT_NO_THROW(simd_legacy_fixture::emit("module"));
}
#endif
