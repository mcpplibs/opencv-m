#include <gtest/gtest.h>
#include <cstring>
#include <string>
#include <vector>

#if defined(MCPP_FEATURE_IMGCODECS)
import mcpplibs.mwrap.opencv.imgcodecs;
import mcpplibs.mwrap.opencv.macros;

#if defined(MCPP_FEATURE_IMGCODECS_WEBP)
namespace {
cv::Mat make_frame(int bias) {
    namespace types = mcpplibs::mwrap::opencv::macros;
    cv::Mat image(8, 8, types::CV_8UC4);
    for (int y = 0; y < image.rows; ++y)
        for (int x = 0; x < image.cols; ++x)
            image.at<cv::Vec4b>(y, x) = cv::Vec4b(20 + x + bias, 60 + y, 100 + bias, 180);
    return image;
}
}

TEST(WebPCodec, LosslessPreservesAlphaAndTransparentRgb) {
    auto image = make_frame(0);
    image.at<cv::Vec4b>(0, 0) = cv::Vec4b(7, 19, 23, 0);
    ASSERT_TRUE(cv::haveImageWriter(".webp"));
    std::vector<uchar> encoded;
    ASSERT_TRUE(cv::imencode(".webp", image, encoded,
        {cv::IMWRITE_WEBP_LOSSLESS_MODE, cv::IMWRITE_WEBP_LOSSLESS_PRESERVE_COLOR}));
    const auto restored = cv::imdecode(encoded, cv::IMREAD_UNCHANGED);
    ASSERT_FALSE(restored.empty());
    ASSERT_EQ(restored.type(), image.type());
    ASSERT_TRUE(restored.size() == image.size());
    EXPECT_DOUBLE_EQ(cv::norm(restored, image, cv::NORM_INF), 0);
}

TEST(WebPCodec, AnimationKeepsFramesTimingAndLoopCount) {
    cv::Animation animation(3);
    animation.frames = {make_frame(0), make_frame(50)};
    animation.durations = {80, 120};
    std::vector<uchar> encoded;
    ASSERT_TRUE(cv::imencodeanimation(".webp", animation, encoded, {cv::IMWRITE_WEBP_QUALITY, 101}));
    cv::Animation restored;
    ASSERT_TRUE(cv::imdecodeanimation(encoded, restored));
    EXPECT_EQ(restored.loop_count, 3);
    EXPECT_EQ(restored.durations, animation.durations);
    ASSERT_EQ(restored.frames.size(), 2u);
    for (int frame = 0; frame < 2; ++frame) {
        ASSERT_EQ(restored.frames[frame].type(), animation.frames[frame].type());
        ASSERT_TRUE(restored.frames[frame].size() == animation.frames[frame].size());
        EXPECT_DOUBLE_EQ(cv::norm(restored.frames[frame], animation.frames[frame], cv::NORM_INF), 0);
    }
}

TEST(WebPCodec, XmpMetadataRoundtripsThroughMuxAndDemux) {
    namespace types = mcpplibs::mwrap::opencv::macros;
    const std::string xmp = "<x:xmpmeta xmlns:x='adobe:ns:meta/'>mwrap</x:xmpmeta>";
    cv::Mat chunk(1, static_cast<int>(xmp.size()), types::CV_8U,
                  const_cast<char*>(xmp.data()));
    const auto image = make_frame(0);
    const std::vector<int> metadata_types{cv::IMAGE_METADATA_XMP};
    const std::vector<cv::Mat> metadata{chunk};
    std::vector<uchar> encoded;
    ASSERT_TRUE(cv::imencodeWithMetadata(".webp", image, metadata_types, metadata, encoded,
        {cv::IMWRITE_WEBP_LOSSLESS_MODE, cv::IMWRITE_WEBP_LOSSLESS_ON}));
    std::vector<int> restored_types;
    std::vector<cv::Mat> restored_metadata;
    const auto restored = cv::imdecodeWithMetadata(encoded, restored_types, restored_metadata, cv::IMREAD_UNCHANGED);
    ASSERT_FALSE(restored.empty());
    EXPECT_DOUBLE_EQ(cv::norm(restored, image, cv::NORM_INF), 0);
    ASSERT_EQ(restored_types, metadata_types);
    ASSERT_EQ(restored_metadata.size(), 1u);
    ASSERT_EQ(restored_metadata[0].total(), xmp.size());
    EXPECT_EQ(std::memcmp(restored_metadata[0].data, xmp.data(), xmp.size()), 0);
}
#else
TEST(WebPCodec, DisabledFeatureDoesNotRegisterTheCodec) {
    EXPECT_FALSE(cv::haveImageWriter(".webp"));
}
#endif
#endif
