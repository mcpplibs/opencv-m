#include <gtest/gtest.h>
#include <vector>

#if defined(MCPP_FEATURE_STITCHING)
import mcpplibs.mwrap.opencv.stitching;
import mcpplibs.mwrap.opencv.macros;

namespace macros = mcpplibs::mwrap::opencv::macros;

TEST(StitchingDetail, CpuBlenderComposesTwoTiles) {
    auto blender = cv::detail::Blender::createDefault(cv::detail::Blender::NO, false);
    blender->prepare(cv::Rect(0, 0, 16, 8));
    cv::Mat left(8, 8, macros::CV_16SC3, cv::Scalar(20, 40, 80));
    cv::Mat right(8, 8, macros::CV_16SC3, cv::Scalar(90, 60, 30));
    cv::Mat mask(8, 8, macros::CV_8UC1, cv::Scalar(255));
    blender->feed(left, mask, cv::Point(0, 0));
    blender->feed(right, mask, cv::Point(8, 0));
    cv::Mat result, result_mask;
    blender->blend(result, result_mask);
    ASSERT_TRUE(result.size() == cv::Size(16, 8));
    EXPECT_EQ(result.type(), macros::CV_16SC3);
    EXPECT_EQ(cv::countNonZero(result_mask), 128);
    EXPECT_DOUBLE_EQ(cv::norm(result(cv::Rect(0, 0, 8, 8)), left, cv::NORM_INF), 0);
    EXPECT_DOUBLE_EQ(cv::norm(result(cv::Rect(8, 0, 8, 8)), right, cv::NORM_INF), 0);
}

TEST(StitchingDetail, CameraIntrinsicsAndPlaneWarp) {
    cv::detail::CameraParams camera;
    camera.focal = 100;
    camera.aspect = 1.5;
    camera.ppx = 8;
    camera.ppy = 4;
    const auto intrinsics = camera.K();
    EXPECT_DOUBLE_EQ(intrinsics.at<double>(0, 0), 100);
    EXPECT_DOUBLE_EQ(intrinsics.at<double>(1, 1), 150);
    EXPECT_DOUBLE_EQ(intrinsics.at<double>(0, 2), 8);
    EXPECT_DOUBLE_EQ(intrinsics.at<double>(1, 2), 4);
    cv::detail::PlaneWarper warper(1);
    cv::Mat identity = cv::Mat::eye(3, 3, macros::CV_32FC1);
    const auto point = warper.warpPoint(cv::Point2f(5, 7), identity, identity);
    EXPECT_NEAR(point.x, 5, 1e-6);
    EXPECT_NEAR(point.y, 7, 1e-6);
}

TEST(StitchingDetail, TimelapserPlacesAFrameInTheGlobalRoi) {
    auto timelapser = cv::detail::Timelapser::createDefault(cv::detail::Timelapser::AS_IS);
    timelapser->initialize(std::vector<cv::Point>{cv::Point(3, 2)}, std::vector<cv::Size>{cv::Size(8, 6)});
    cv::Mat frame(6, 8, macros::CV_16SC3, cv::Scalar(15, 30, 45));
    cv::Mat mask(6, 8, macros::CV_8UC1, cv::Scalar(255));
    timelapser->process(frame, mask, cv::Point(3, 2));
    cv::Mat restored;
    timelapser->getDst().copyTo(restored);
    EXPECT_TRUE(restored.size() == frame.size());
    EXPECT_DOUBLE_EQ(cv::norm(restored, frame, cv::NORM_INF), 0);
}
#endif
