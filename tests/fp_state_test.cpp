#include <gtest/gtest.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
import mcpplibs.mwrap.opencv.core;
#if defined(MCPP_FEATURE_VIDEOIO)
import mcpplibs.mwrap.opencv.videoio;
#endif
#include "fixtures/fp_state_fixture.hpp"
TEST(FpState, OriginalQualifiedApiSignaturesStateAndScopeRestoration) {
    EXPECT_NO_THROW(fp_state_fixture::emit("module"));
}
