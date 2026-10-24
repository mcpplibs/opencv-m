#include <gtest/gtest.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#if defined(MCPP_FEATURE_DNN)
import mcpplibs.mwrap.opencv.dnn;
#include "fixtures/dnn_compat_fixture.hpp"
TEST(DnnCompat, OriginalEqualitySignaturesAndUnavailableBackendContracts) {
    EXPECT_NO_THROW(dnn_compat_fixture::emit("module"));
}
#endif
