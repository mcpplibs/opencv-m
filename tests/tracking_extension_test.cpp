#include <gtest/gtest.h>
#include <memory>
#include <vector>

#if defined(MCPP_FEATURE_VIDEO)
import mcpplibs.mwrap.opencv.video;
import mcpplibs.mwrap.opencv.macros;
namespace types = mcpplibs::mwrap::opencv::macros;

namespace {
class MeanFeature final : public cv::detail::tracking::TrackerFeature {
public:
    int calls = 0;
protected:
    bool computeImpl(const std::vector<cv::Mat>& images, cv::Mat& response) override {
        ++calls;
        response.create(1, static_cast<int>(images.size()), types::CV_32F);
        for (int index = 0; index < response.cols; ++index)
            response.at<float>(0, index) = static_cast<float>(cv::mean(images[index])[0]);
        return true;
    }
};
}

TEST(TrackingExtension, UserFeatureRunsThroughTheSdkFeatureSet) {
    auto feature = cv::makePtr<MeanFeature>();
    cv::detail::tracking::TrackerFeatureSet set;
    ASSERT_TRUE(set.addTrackerFeature(feature));
    std::vector<cv::Mat> images{
        cv::Mat(4, 4, types::CV_8U, cv::Scalar(9)),
        cv::Mat(4, 4, types::CV_8U, cv::Scalar(17))};
    set.extraction(images);
    EXPECT_EQ(feature->calls, 1);
    ASSERT_EQ(set.getTrackerFeatures().size(), 1u);
    ASSERT_EQ(set.getResponses().size(), 1u);
    const auto& response = set.getResponses()[0];
    ASSERT_EQ(response.cols, 2);
    EXPECT_FLOAT_EQ(response.at<float>(0, 0), 9);
    EXPECT_FLOAT_EQ(response.at<float>(0, 1), 17);
    EXPECT_THROW(set.addTrackerFeature(feature), cv::Exception);
}

TEST(TrackingExtension, TargetStateAndDefaultSamplerExecuteSdkMethods) {
    auto state = cv::makePtr<cv::detail::tracking::TrackerTargetState>();
    state->setTargetPosition(cv::Point2f(3, 5));
    state->setTargetWidth(7);
    state->setTargetHeight(11);
    EXPECT_EQ(state->getTargetPosition(), cv::Point2f(3, 5));
    EXPECT_EQ(state->getTargetWidth(), 7);
    EXPECT_EQ(state->getTargetHeight(), 11);
    cv::detail::tracking::ConfidenceMap confidence{{state, 0.75f}};
    cv::detail::tracking::Trajectory trajectory{state};
    EXPECT_EQ(confidence[0].first.get(), trajectory[0].get());
    cv::detail::tracking::TrackerSamplerCSC sampler;
    sampler.setMode(cv::detail::tracking::TrackerSamplerCSC::MODE_INIT_POS);
    cv::Mat image(32, 32, types::CV_8U, cv::Scalar(23));
    std::vector<cv::Mat> samples;
    // Pinned OpenCV returns false even after populating samples. Preserve
    // that SDK behavior; do not "repair" the return value in the wrapper.
    EXPECT_FALSE(sampler.sampling(image, cv::Rect(12, 12, 4, 4), samples));
    ASSERT_FALSE(samples.empty());
    for (const auto& sample : samples) {
        EXPECT_EQ(sample.size(), cv::Size(4, 4));
        EXPECT_DOUBLE_EQ(cv::mean(sample)[0], 23);
    }
}
#endif
