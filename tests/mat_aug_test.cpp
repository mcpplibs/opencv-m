#include <gtest/gtest.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
import mcpplibs.mwrap.opencv.core;
#include "fixtures/mat_aug_fixture.hpp"
TEST(MatAug, OriginalSignaturesValuesAndReferenceIdentity) {
    EXPECT_NO_THROW(mat_aug_fixture::emit("module"));
}
