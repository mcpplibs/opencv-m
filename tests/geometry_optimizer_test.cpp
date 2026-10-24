#include <gtest/gtest.h>
#include <memory>

#if defined(MCPP_FEATURE_GEOMETRY)
import mcpplibs.mwrap.opencv.geometry;

namespace {
class RejectingBackend final : public cv::detail::LevMarqBackend {
public:
    int prepared = 0, evaluated = 0;
    bool enableGeo() override { return false; }
    void prepareVars() override { ++prepared; }
    bool calcFunc(double&, bool, bool) override { ++evaluated; return false; }
    void currentOplusX(const cv::Mat_<double>&, bool) override { ADD_FAILURE(); }
    const cv::Mat_<double> getJtb() override { ADD_FAILURE(); return {}; }
    const cv::Mat_<double> getDiag() override { ADD_FAILURE(); return {}; }
    void setDiag(const cv::Mat_<double>&) override { ADD_FAILURE(); }
    void doJacobiScaling(const cv::Mat_<double>&) override { ADD_FAILURE(); }
    bool decompose() override { ADD_FAILURE(); return false; }
    bool solveDecomposed(const cv::Mat_<double>&, cv::Mat_<double>&) override { ADD_FAILURE(); return false; }
    bool calcJtbv(cv::Mat_<double>&) override { ADD_FAILURE(); return false; }
    void acceptProbe() override { ADD_FAILURE(); }
};
}

TEST(GeometryOptimizer, CustomBackendRejectionPreservesTheSdkReport) {
    auto backend = cv::makePtr<RejectingBackend>();
    cv::LevMarq::Settings settings;
    settings.setGeodesic(false);
    cv::detail::LevMarqBase optimizer(backend, settings);
    const auto report = optimizer.optimize();
    EXPECT_FALSE(report.found);
    EXPECT_EQ(report.iters, 0);
    EXPECT_DOUBLE_EQ(report.energy, 0);
    EXPECT_EQ(backend->prepared, 1);
    EXPECT_EQ(backend->evaluated, 1);
}

TEST(GeometryOptimizer, UnsupportedGeodesicStopsBeforeEvaluatingTheBackend) {
    auto backend = cv::makePtr<RejectingBackend>();
    cv::LevMarq::Settings settings;
    settings.setGeodesic(true);
    cv::detail::LevMarqBase optimizer(backend, settings);
    EXPECT_FALSE(optimizer.optimize().found);
    EXPECT_EQ(backend->prepared, 0);
    EXPECT_EQ(backend->evaluated, 0);
}
#endif
