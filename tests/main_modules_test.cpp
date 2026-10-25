// Exercise the newly integrated main modules through the aggregate module.
// Inputs are generated locally; no camera, model or image download is needed.
#include <gtest/gtest.h>
#include <limits>
#include <vector>

#if defined(MCPP_FEATURE_FEATURES)
import mcpplibs.mwrap.opencv.features;
#endif
#if defined(MCPP_FEATURE_OBJDETECT)
import mcpplibs.mwrap.opencv.objdetect;
#endif
#if defined(MCPP_FEATURE_STEREO)
import mcpplibs.mwrap.opencv.stereo;
#endif
#if defined(MCPP_FEATURE_CALIB)
import mcpplibs.mwrap.opencv.calib;
#endif
#if defined(MCPP_FEATURE_PHOTO)
import mcpplibs.mwrap.opencv.photo;
#endif
#if defined(MCPP_FEATURE_PTCLOUD)
import mcpplibs.mwrap.opencv.ptcloud;
#endif
#if defined(MCPP_FEATURE_STITCHING)
import mcpplibs.mwrap.opencv.stitching;
#endif
#if defined(MCPP_FEATURE_VIDEO)
import mcpplibs.mwrap.opencv.video;
#endif
import mcpplibs.mwrap.opencv;
namespace types = mcpplibs::mwrap::opencv::macros;

#if defined(MCPP_FEATURE_PHOTO)
TEST(MainModules, OriginalGlobalInpaintingEnumRemainsAvailable) {
    InpaintingModes mode = CV_INPAINT_TELEA;
    EXPECT_EQ(static_cast<int>(mode), cv::INPAINT_TELEA);
    EXPECT_EQ(static_cast<int>(CV_INPAINT_NS), cv::INPAINT_NS);
}
#endif
#if defined(MCPP_FEATURE_VIDEO)
TEST(MainModules, OriginalGlobalOpticalFlowFlagsRemainAvailable) {
    EXPECT_EQ(CV_LKFLOW_PYR_A_READY, 1);
    EXPECT_EQ(CV_LKFLOW_PYR_B_READY, 2);
    EXPECT_EQ(static_cast<int>(CV_LKFLOW_INITIAL_GUESSES), cv::OPTFLOW_USE_INITIAL_FLOW);
    EXPECT_EQ(static_cast<int>(CV_LKFLOW_GET_MIN_EIGENVALS), cv::OPTFLOW_LK_GET_MIN_EIGENVALS);
}
#endif

#if defined(MCPP_FEATURE_PTCLOUD)
TEST(MainModules, PointCloudCpuRenderingAndInvalidPoints) {
    cv::Mat points(1, 2, types::CV_32FC4, cv::Scalar(0, 0, 1, 0));
    cv::Mat normals(1, 2, types::CV_32FC4, cv::Scalar(0, 0, -1, 0));
    cv::Mat colors(1, 2, types::CV_32FC4, cv::Scalar(12, 34, 56, 0));
    points.at<cv::Vec4f>(0, 1)[0] = std::numeric_limits<float>::quiet_NaN();
    cv::Mat image;
    cv::detail::renderPointsNormalsColors(points, normals, colors, image);
    ASSERT_EQ(image.type(), types::CV_8UC4);
    EXPECT_EQ(image.size(), points.size());
    EXPECT_EQ(image.at<cv::Vec4b>(0, 0), cv::Vec4b(12, 34, 56, 0));
    EXPECT_EQ(image.at<cv::Vec4b>(0, 1), cv::Vec4b(0, 32, 0, 0));
    cv::detail::renderPointsNormals(points, normals, image, cv::Vec3f(0, 0, -1));
    const auto shaded = image.at<cv::Vec4b>(0, 0);
    EXPECT_GT(shaded[0], 200);
    EXPECT_EQ(shaded[0], shaded[1]);
    EXPECT_EQ(shaded[1], shaded[2]);
    EXPECT_EQ(shaded[3], 0);
    EXPECT_EQ(image.at<cv::Vec4b>(0, 1), cv::Vec4b(0, 32, 0, 0));
    EXPECT_THROW(cv::detail::renderPointsNormals(points, cv::Mat(), image, cv::Vec3f()), cv::Exception);
}
#endif

#if defined(MCPP_FEATURE_FEATURES)
TEST(MainModules, FeaturesSiftDescriptors) {
    cv::Mat image(128, 128, types::CV_8UC1, cv::Scalar(0));
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 8; ++x)
            if ((x + y) % 2)
                cv::rectangle(image, cv::Rect(x * 16, y * 16, 16, 16), cv::Scalar(255), -1);
    auto sift = cv::SIFT::create(32);
    std::vector<cv::KeyPoint> points;
    cv::Mat descriptors;
    sift->detectAndCompute(image, cv::noArray(), points, descriptors);
    ASSERT_FALSE(points.empty());
    EXPECT_EQ(descriptors.rows, static_cast<int>(points.size()));
    EXPECT_EQ(descriptors.cols, 128);
}
#endif

#if defined(MCPP_FEATURE_OBJDETECT)
TEST(MainModules, ObjdetectQrEncoder) {
    cv::Mat code;
    cv::QRCodeEncoder::create()->encode("mwrap", code);
    ASSERT_FALSE(code.empty());
    EXPECT_EQ(code.rows, code.cols);
    EXPECT_GT(code.rows, 20);
    EXPECT_GT(cv::countNonZero(code), 0);
    EXPECT_LT(cv::countNonZero(code), code.rows * code.cols);
}
#endif

#if defined(MCPP_FEATURE_STEREO)
TEST(MainModules, StereoBlockMatching) {
    cv::Mat left(64, 96, types::CV_8UC1);
    cv::randu(left, 0, 255);
    cv::Mat disparity;
    cv::StereoBM::create(16, 5)->compute(left, left, disparity);
    EXPECT_EQ(disparity.rows, left.rows);
    EXPECT_EQ(disparity.cols, left.cols);
    EXPECT_EQ(disparity.type(), types::CV_16SC1);
}
#endif

#if defined(MCPP_FEATURE_CALIB)
TEST(MainModules, CalibCameraInitializationFromProjectedViews) {
    cv::Mat intrinsic = cv::Mat::eye(3, 3, types::CV_64F);
    intrinsic.at<double>(0, 0) = intrinsic.at<double>(1, 1) = 500;
    intrinsic.at<double>(0, 2) = 319.5;
    intrinsic.at<double>(1, 2) = 239.5;
    std::vector<std::vector<cv::Point3f>> objects(3);
    std::vector<std::vector<cv::Point2f>> images(3);
    for (int view = 0; view < 3; ++view) {
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 5; ++x)
                objects[view].emplace_back(x * 0.1f, y * 0.1f, 0.f);
        cv::projectPoints(objects[view], cv::Vec3d(0.1 + view * 0.1, 0.2, 0.05),
                          cv::Vec3d(0.1, -0.1, 2.0 + view), intrinsic, cv::noArray(), images[view]);
    }
    const auto estimated = cv::initCameraMatrix2D(objects, images, cv::Size(640, 480));
    EXPECT_NEAR(estimated.at<double>(0, 0), 500.0, 0.2);
    EXPECT_NEAR(estimated.at<double>(1, 1), 500.0, 0.2);
    EXPECT_DOUBLE_EQ(estimated.at<double>(0, 2), 319.5);
}
#endif

#if defined(MCPP_FEATURE_PHOTO)
TEST(MainModules, PhotoDenoiseConstantImage) {
    cv::Mat image(24, 24, types::CV_8UC1, cv::Scalar(73)), denoised;
    cv::fastNlMeansDenoising(image, denoised, 3.f, 3, 7);
    EXPECT_TRUE(denoised.size() == image.size());
    EXPECT_EQ(cv::norm(image, denoised, cv::NORM_INF), 0.0);
}
#endif

#if defined(MCPP_FEATURE_PTCLOUD)
TEST(MainModules, PtcloudDepthToThreeDimensions) {
    cv::Mat depth(2, 3, types::CV_32F, cv::Scalar(1));
    cv::Mat intrinsic = cv::Mat::eye(3, 3, types::CV_32F), cloud;
    cv::depthTo3d(depth, intrinsic, cloud);
    ASSERT_EQ(cloud.type(), types::CV_32FC4);
    const auto point = cloud.at<cv::Vec4f>(1, 2);
    EXPECT_FLOAT_EQ(point[0], 2.f);
    EXPECT_FLOAT_EQ(point[1], 1.f);
    EXPECT_FLOAT_EQ(point[2], 1.f);
    EXPECT_FLOAT_EQ(point[3], 0.f);
}
#endif

#if defined(MCPP_FEATURE_STITCHING)
TEST(MainModules, StitchingRejectsInsufficientImages) {
    auto stitcher = cv::Stitcher::create();
    std::vector<cv::Mat> images{cv::Mat(32, 32, types::CV_8UC3, cv::Scalar(10, 20, 30))};
    cv::Mat panorama;
    EXPECT_EQ(static_cast<int>(stitcher->stitch(images, panorama)),
              static_cast<int>(cv::Stitcher::ERR_NEED_MORE_IMGS));
}
#endif

#if defined(MCPP_FEATURE_VIDEO)
TEST(MainModules, VideoKalmanPredictionAndOpticalFlow) {
    cv::KalmanFilter filter(2, 1);
    filter.transitionMatrix = cv::Mat::eye(2, 2, types::CV_32F);
    filter.statePost = cv::Mat(2, 1, types::CV_32F, cv::Scalar(5));
    const auto predicted = filter.predict();
    EXPECT_FLOAT_EQ(predicted.at<float>(0, 0), 5.f);
    cv::Mat image(32, 32, types::CV_8UC1, cv::Scalar(50)), flow;
    cv::calcOpticalFlowFarneback(image, image, flow, .5, 1, 5, 1, 5, 1.1, 0);
    EXPECT_EQ(flow.type(), types::CV_32FC2);
    EXPECT_LE(cv::norm(flow, cv::NORM_INF), 1e-6);
}
#endif

#if defined(MCPP_FEATURE_GEOMETRY)
TEST(MainModules, GeometryBuildsTheMinimumSpanningTree) {
    const std::vector<cv::MSTEdge> edges{{0, 1, 1}, {1, 2, 2}, {0, 2, 9}};
    for (const auto algorithm : {cv::MST_PRIM, cv::MST_KRUSKAL}) {
        std::vector<cv::MSTEdge> tree;
        ASSERT_TRUE(cv::buildMST(3, edges, tree, algorithm));
        ASSERT_EQ(tree.size(), 2u);
        double weight = 0;
        for (const auto& edge : tree) weight += edge.weight;
        EXPECT_DOUBLE_EQ(weight, 3);
    }
}
#endif

#if defined(MCPP_FEATURE_FEATURES) && defined(MCPP_FEATURE_DNN)
TEST(MainModules, DiskApiRejectsAnAbsentModel) {
    EXPECT_THROW(cv::DISK::create(""), cv::Exception);
}
#endif
