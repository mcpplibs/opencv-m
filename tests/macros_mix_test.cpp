// The optional macros side header must coexist with the import in one TU.
// In such a mixed TU the RAW macro spellings are the ones to use (an active
// CV_8UC3 macro would mangle the mcpplibs::mwrap::opencv::macros::CV_8UC3 spelling — that constexpr
// surface is for module-only TUs, see api_surface_test.cpp).
#include <gtest/gtest.h>

// Canonical mixing order: textual includes BEFORE the import.
#include <mwrap/opencv/macros.hpp>

import mcpplibs.mwrap.opencv;

TEST(MacrosMix, AssertAndTypeMacros) {
    cv::Mat m(2, 2, CV_8UC1, cv::Scalar(7));
    CV_Assert(!m.empty());
    EXPECT_EQ(m.type(), CV_8UC1);
    EXPECT_EQ(CV_MAKETYPE(CV_8U, 3), CV_8UC3);
}

TEST(MacrosMix, VersionMacros) {
    EXPECT_EQ(CV_VERSION_MAJOR, 5);
    EXPECT_EQ(cv::getVersionMajor(), CV_VERSION_MAJOR);
}

TEST(MacrosMix, GlobalFastMathCoexistsWithTextualHeaders) {
    EXPECT_EQ(::cvRound(2.5), 2);
    EXPECT_EQ(::cvFloor(-2.5f), -3);
    EXPECT_EQ(::cvCeil(2), 2);
    EXPECT_EQ(::CV_FOURCC('M', 'J', 'P', 'G'), CV_FOURCC_MACRO('M', 'J', 'P', 'G'));
}

TEST(MacrosMix, SubmatrixMacroMatchesMatFlags) {
    cv::Mat image(8, 10, CV_8UC1);
    cv::Mat roi = image(cv::Rect(2, 2, 4, 3));
    EXPECT_EQ(CV_IS_SUBMAT(image.flags) != 0, image.isSubmatrix());
    EXPECT_EQ(CV_IS_SUBMAT(roi.flags) != 0, roi.isSubmatrix());
    EXPECT_TRUE(roi.isSubmatrix());
}
