#include <gtest/gtest.h>
#include <string>

import mcpplibs.mwrap.opencv;

#if defined(MCPP_FEATURE_HIGHGUI_NATIVE) || defined(MCPP_FEATURE_HIGHGUI_GTK3) || defined(MCPP_FEATURE_HIGHGUI_WIN32) || defined(MCPP_FEATURE_HIGHGUI_COCOA)
TEST(HighguiNative, CreateDisplayProcessAndDestroyWindow) {
    ASSERT_FALSE(cv::currentUIFramework().empty());
    const std::string name = "mwrap-native-window-test";
    cv::Mat image(48, 64, mcpplibs::mwrap::opencv::macros::CV_8UC3, cv::Scalar(20, 90, 160));
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    cv::imshow(name, image);
    cv::waitKey(40);
    const auto rect = cv::getWindowImageRect(name);
    EXPECT_GT(rect.width, 0);
    EXPECT_GT(rect.height, 0);
    cv::destroyWindow(name);
    cv::waitKey(10);
}
#endif
