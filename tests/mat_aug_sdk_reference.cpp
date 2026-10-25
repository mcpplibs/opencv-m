#include <gtest/gtest.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <opencv2/core.hpp>
#include "fixtures/mat_aug_fixture.hpp"
TEST(MatAugSdk, OriginalSignaturesValuesAndReferenceIdentity) {
    EXPECT_NO_THROW(mat_aug_fixture::emit("sdk"));
}
