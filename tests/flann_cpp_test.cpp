#include <gtest/gtest.h>
#include <type_traits>
#include <vector>

#if defined(MCPP_FEATURE_FLANN)
import mcpplibs.mwrap.opencv.flann;
import mcpplibs.mwrap.opencv.macros;

TEST(FlannCpp, GenericIndexFindsTheNearestFloatVector) {
    static_assert(std::is_same_v<cv::flann::L2<float>, cvflann::L2<float>>);
    std::vector<float> points{0, 0, 1, 0, 4, 4, 9, 9};
    cv::Mat dataset(4, 2, mcpplibs::mwrap::opencv::macros::CV_32F, points.data());
    cv::flann::GenericIndex<cv::flann::L2<float>> index(dataset, cvflann::LinearIndexParams());
    std::vector<float> query{0.1f, 0.2f}, distances(1);
    std::vector<int> indices(1);
    index.knnSearch(query, indices, distances, 1, cvflann::SearchParams(cvflann::FLANN_CHECKS_UNLIMITED));
    EXPECT_EQ(indices[0], 0);
    EXPECT_NEAR(distances[0], 0.05f, 1e-6f);
}

TEST(FlannCpp, LowLevelNamespaceAndMatrixViewRemainAvailable) {
    std::vector<float> points{0, 0, 1, 0, 4, 4, 9, 9};
    cvflann::Matrix<float> dataset(points.data(), 4, 2);
    cvflann::Index<cvflann::L2<float>> index(dataset, cvflann::LinearIndexParams());
    index.buildIndex();
    float query_data[]{3.9f, 4.1f};
    int index_data[]{-1};
    float distance_data[]{-1};
    cvflann::Matrix<float> query(query_data, 1, 2), distances(distance_data, 1, 1);
    cvflann::Matrix<int> indices(index_data, 1, 1);
    index.knnSearch(query, indices, distances, 1, cvflann::SearchParams(cvflann::FLANN_CHECKS_UNLIMITED));
    EXPECT_EQ(index_data[0], 2);
    EXPECT_NEAR(distance_data[0], 0.02f, 1e-6f);
    EXPECT_EQ(dataset.rows, 4u);
    EXPECT_EQ(dataset.cols, 2u);
    const float a[]{1, 2}, b[]{2, 4};
    EXPECT_FLOAT_EQ(cvflann::L1<float>()(a, b, 2), 3);
    EXPECT_FLOAT_EQ(cvflann::L2<float>()(a, b, 2), 5);
}
#endif
