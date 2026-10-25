#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#if defined(MCPP_FEATURE_DNN)
import mcpplibs.mwrap.opencv.dnn;
import mcpplibs.mwrap.opencv.macros;
namespace types = mcpplibs::mwrap::opencv::macros;

TEST(DnnShapes, ShapeConstructionProductsAndAxisContracts) {
    const int sizes[]{2, 3, 4};
    const auto dimensions = cv::dnn::shape(sizes, 3);
    ASSERT_EQ(dimensions.dims, 3);
    EXPECT_EQ(cv::dnn::total(dimensions), 24u);
    EXPECT_EQ(cv::dnn::total(dimensions, 1, 3), 12u);
    const auto combined = cv::dnn::concat(cv::dnn::shape(2, 3), cv::dnn::shape(4));
    EXPECT_TRUE(dimensions == combined);
    EXPECT_TRUE(cv::dnn::isAllOnes(cv::dnn::shape(2, 1, 1), 1, 3));
    EXPECT_FALSE(cv::dnn::isAllOnes(dimensions, 0, 3));
    EXPECT_EQ(cv::dnn::normalize_axis(-1, dimensions), 2);
    EXPECT_EQ(cv::dnn::normalize_axis(-3, 3), 0);
    EXPECT_THROW(cv::dnn::normalize_axis(4, 3), cv::Exception);
    EXPECT_THROW(cv::dnn::total(dimensions, 2, 1), cv::Exception);
    const auto all = cv::dnn::normalize_axis_range(cv::Range::all(), 5);
    EXPECT_EQ(all.start, 0);
    EXPECT_EQ(all.end, 5);
    const auto tail = cv::dnn::normalize_axis_range(cv::Range(1, -2), 5);
    EXPECT_EQ(tail.start, 1);
    EXPECT_EQ(tail.end, 4);
}

TEST(DnnShapes, SlicesAndPlanesShareTheirOriginalTensorStorage) {
    const int sizes[]{2, 3, 4, 5};
    cv::Mat tensor(4, sizes, types::CV_32F, cv::Scalar(7));
    const auto dimensions = cv::dnn::shape(tensor);
    EXPECT_EQ(cv::dnn::total(tensor), 120u);
    EXPECT_EQ(cv::dnn::total(tensor, 2), 20u);
    EXPECT_TRUE(cv::dnn::shape(tensor.getUMat(cv::ACCESS_READ)) == dimensions);
    const cv::dnn::_Range first(1), second(1), third(1, 2), fourth(2, 2);
    EXPECT_EQ(cv::dnn::slice(tensor, first).total(), 60u);
    EXPECT_EQ(cv::dnn::slice(tensor, first, second).total(), 20u);
    EXPECT_EQ(cv::dnn::slice(tensor, first, second, third).total(), 10u);
    EXPECT_EQ(cv::dnn::slice(tensor, first, second, third, fourth).total(), 4u);
    auto plane = cv::dnn::getPlane(tensor, 1, 2);
    ASSERT_EQ(plane.rows, 4);
    ASSERT_EQ(plane.cols, 5);
    plane.at<float>(0, 0) = 9;
    EXPECT_FLOAT_EQ(tensor.ptr<float>(1, 2)[0], 9);
    EXPECT_EQ(cv::dnn::total(cv::Mat()), 0u);
}

TEST(DnnShapes, GenericFormattingRetainsTheSdkTextAndStream) {
    const std::vector<float> dimensions{1, 2};
    EXPECT_EQ(cv::dnn::toString(dimensions), "[ 1 2 ]");
    EXPECT_EQ(cv::dnn::toString(dimensions, "shape"), "shape [ 1 2 ]");
    std::ostringstream text;
    EXPECT_EQ(&cv::dnn::operator<<(text, dimensions), &text);
    EXPECT_EQ(text.str(), "[ 1 2 ]");
    const auto formatted = cv::dnn::toString(cv::dnn::shape(2, 3), "tensor");
    EXPECT_EQ(formatted.find("tensor "), 0u);
    EXPECT_NE(formatted.find('2'), std::string::npos);
    EXPECT_NE(formatted.find('3'), std::string::npos);
    testing::internal::CaptureStdout();
    cv::dnn::print(dimensions, "shape");
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "shape [ 1 2 ]\n");
}
#endif
