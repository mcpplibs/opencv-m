// Self-contained imgcodecs roundtrip through the module layer: synthesize an
// image, encode to PNG (lossless — pixel-exact back) and JPEG (lossy — close),
// decode, compare.
#include <gtest/gtest.h>

#include <vector>

import mcpplibs.mwrap.opencv;

#if defined(MCPP_FEATURE_IMGPROC) && defined(MCPP_FEATURE_IMGCODECS)
namespace {

cv::Mat make_test_image() {
    cv::Mat img(64, 64, mcpplibs::mwrap::opencv::macros::CV_8UC3, cv::Scalar(30, 60, 90));
    cv::circle(img, {32, 32}, 20, {255, 255, 255}, -1);
    cv::rectangle(img, {4, 4}, {60, 60}, {0, 128, 255}, 2);
    return img;
}

} // namespace

#if defined(MCPP_FEATURE_IMGCODECS_PNG)
TEST(Roundtrip, PngIsPixelExact) {
    cv::Mat img = make_test_image();
    std::vector<unsigned char> buf;
    ASSERT_TRUE(cv::imencode(".png", img, buf));
    cv::Mat back = cv::imdecode(buf, cv::IMREAD_COLOR);
    ASSERT_FALSE(back.empty());
    ASSERT_TRUE(back.size() == img.size());
    cv::Mat diff;
    cv::absdiff(img, back, diff);
    EXPECT_EQ(cv::countNonZero(diff.reshape(1)), 0);
}
#endif

#if defined(MCPP_FEATURE_IMGCODECS_JPEG)
TEST(Roundtrip, JpegIsClose) {
    cv::Mat img = make_test_image();
    std::vector<unsigned char> buf;
    ASSERT_TRUE(cv::imencode(".jpg", img, buf, {cv::IMWRITE_JPEG_QUALITY, 95}));
    cv::Mat back = cv::imdecode(buf, cv::IMREAD_COLOR);
    ASSERT_FALSE(back.empty());
    ASSERT_TRUE(back.size() == img.size());
    EXPECT_LT(cv::norm(img, back, cv::NORM_L1) / (64.0 * 64 * 3), 12.0);  // q95 ringing on sharp synthetic edges ≈ 7
}
#endif

#endif
