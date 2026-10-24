#include <gtest/gtest.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
import mcpplibs.mwrap.opencv.core;
#include "fixtures/matx_templates_fixture.hpp"
TEST(MatxTemplates, OriginalExplicitArgumentsPrefixesAndDerivedDeduction) {
    EXPECT_NO_THROW(matx_templates_fixture::emit("module"));
}
