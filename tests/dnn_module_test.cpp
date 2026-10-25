// dnn feature test — the opencv.dnn MODULE interface (not textual headers).
// Only built with `--features dnn` (which adds the vendored dnn sources and
// compiles src/dnn.cppm); without the feature this TU has no tests, so the
// default `mcpp test` stays a clean pass. Deliberately not named dnn.cpp
// (would collide with the dep's modules/dnn/src/dnn.cpp — #240 family).
#include <gtest/gtest.h>
#if defined(MCPP_FEATURE_DNN)
#include "fixtures/ort_model.hpp"
import mcpplibs.mwrap.opencv;
import mcpplibs.mwrap.opencv.dnn;

// blobFromImage through the module layer must yield a correctly shaped/valued
// NCHW blob (exercises the dnn core + the whole protobuf/mlas link closure),
// and an empty Net must construct.
TEST(DnnModule, BlobFromImageAndNet) {
    cv::Mat img(32, 32, mcpplibs::mwrap::opencv::macros::CV_8UC3, cv::Scalar(10, 20, 30));
    cv::Mat blob = cv::dnn::blobFromImage(img, 1.0 / 255.0, cv::Size(16, 16),
                                          cv::Scalar(), true, false);
    ASSERT_EQ(blob.dims, 4);
    EXPECT_EQ(blob.size[0], 1);
    EXPECT_EQ(blob.size[1], 3);
    EXPECT_EQ(blob.size[2], 16);
    EXPECT_EQ(blob.size[3], 16);
    EXPECT_NEAR(blob.ptr<float>(0)[0], 30.0f / 255.0f, 1e-3f);  // R after swap

    cv::dnn::Net net;
    EXPECT_TRUE(net.empty());
}

namespace {
void verify_native_engine(int engine) {
    // The buffer overload honors ENGINE_CLASSIC directly. No external runtime
    // is requested, so these cases also run on the musl/main-module matrix.
    auto net = cv::dnn::readNetFromONNX(reinterpret_cast<const char*>(opencv_ort_model),
                                     sizeof(opencv_ort_model), engine);
    ASSERT_FALSE(net.empty());
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    cv::Mat input(1, 3, mcpplibs::mwrap::opencv::macros::CV_32F);
    for (int i = 0; i < 3; ++i) input.at<float>(0, i) = float(i + 1);
    net.setInput(input, "x");
    const auto output = net.forward("y");
    ASSERT_EQ(output.total(), 3u);
    for (int i = 0; i < 3; ++i) EXPECT_FLOAT_EQ(output.ptr<float>()[i], float(2 * (i + 1)));
}
}

TEST(DnnModule, ClassicCpuEngineExecutesOnnxAdd) { verify_native_engine(cv::dnn::ENGINE_CLASSIC); }
TEST(DnnModule, NewCpuEngineExecutesOnnxAdd) { verify_native_engine(cv::dnn::ENGINE_NEW); }
TEST(DnnModule, PublicDispatchedActivationFunctionRunsRelu) {
    cv::dnn::ActivationFunc relu = cv::dnn::getActivationFunc(cv::dnn::ACTIV_RELU);
    ASSERT_NE(relu, nullptr);
    const float input[]{-3, -1, 0, 2, 4};
    float output[5]{};
    relu(input, output, 5, nullptr);
    const float expected[]{0, 0, 0, 2, 4};
    for (int i = 0; i < 5; ++i) EXPECT_FLOAT_EQ(output[i], expected[i]);
}
#endif
