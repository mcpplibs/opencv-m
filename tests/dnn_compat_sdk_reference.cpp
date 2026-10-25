#include <gtest/gtest.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#if defined(MCPP_FEATURE_DNN)
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/utils/inference_engine.hpp>
#include "fixtures/dnn_compat_fixture.hpp"
TEST(DnnCompatSdk, OriginalEqualitySignaturesAndUnavailableBackendContracts) {
    EXPECT_NO_THROW(dnn_compat_fixture::emit("sdk"));
}
#endif
