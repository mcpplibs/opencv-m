#include <gtest/gtest.h>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

import mcpplibs.mwrap.opencv.core;
#include "fixtures/utility_helpers_fixture.hpp"

TEST(UtilityHelpers, NormAlignmentRawArrayGeometryAndScalarContracts) {
    EXPECT_NO_THROW(utility_helpers_fixture::emit("module"));
}
