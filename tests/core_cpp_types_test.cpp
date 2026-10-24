#include <gtest/gtest.h>
#include <chrono>
#include <filesystem>
#include <memory>
#include <thread>
#include <type_traits>
#include <vector>

import mcpplibs.mwrap.opencv.core;
import mcpplibs.mwrap.opencv.macros;

TEST(CoreCppTypes, UpstreamGlobalAndCppAliasesRemainAvailable) {
    static_assert(std::is_same_v<uchar, unsigned char>);
    static_assert(std::is_same_v<schar, signed char>);
    static_assert(std::is_same_v<ushort, unsigned short>);
    static_assert(sizeof(int64) == 8 && sizeof(uint64) == 8);
    static_assert(std::is_same_v<cv::int64_t, std::int64_t>);
    static_assert(sizeof(cv::Vec3l::value_type) == 8);
    static_assert(sizeof(cv::bfloat) == 2 && sizeof(cv::hfloat) == 2);
    static_assert(CPU_AVX512_ICL > CPU_AVX2 && CPU_RVV_ZVFH > CPU_RVV);
    Cv32suf bits{};
    bits.f = 1.0f;
    EXPECT_EQ(bits.u, 0x3f800000u);
    cv::MatND matrix(2, 3, mcpplibs::mwrap::opencv::macros::CV_8UC1);
    EXPECT_EQ(matrix.total(), 6u);
}

TEST(CoreCppTypes, AffineCompositionPointsVectorsAndInverse) {
    const cv::Affine3d translation(cv::Vec3d(0, 0, 0), cv::Vec3d(1, 2, 3));
    const auto vector = translation * cv::Vec3d(4, 5, 6);
    const auto point = translation * cv::Point3d(4, 5, 6);
    EXPECT_DOUBLE_EQ(vector[0], 5);
    EXPECT_DOUBLE_EQ(vector[1], 7);
    EXPECT_DOUBLE_EQ(vector[2], 9);
    EXPECT_DOUBLE_EQ(point.x, 5);
    EXPECT_DOUBLE_EQ(point.y, 7);
    EXPECT_DOUBLE_EQ(point.z, 9);
    const auto identity = translation.inv() * translation;
    const auto restored = identity * cv::Vec3d(4, 5, 6);
    EXPECT_NEAR(restored[0], 4, 1e-12);
    EXPECT_NEAR(restored[1], 5, 1e-12);
    EXPECT_NEAR(restored[2], 6, 1e-12);
}

TEST(CoreCppTypes, QuaternionRotationAndGrowingAutoBuffer) {
    const auto q = cv::Quatd::createFromAngleAxis(
        mcpplibs::mwrap::opencv::macros::CV_PI / 2, cv::Vec3d(0, 0, 1));
    const auto rotated = q.toRotMat3x3() * cv::Vec3d(1, 0, 0);
    EXPECT_NEAR(rotated[0], 0, 1e-12);
    EXPECT_NEAR(rotated[1], 1, 1e-12);
    EXPECT_NEAR(rotated[2], 0, 1e-12);
    cv::AutoBuffer<int, 4> buffer(3, 7);
    buffer.resize(12);
    EXPECT_EQ(buffer.size(), 12u);
    EXPECT_EQ(buffer.data()[2], 7);
    buffer.data()[11] = 42;
    EXPECT_EQ(buffer.data()[11], 42);
    std::vector<int> labels;
    const int classes = cv::partition(std::vector<int>{1, 2, 1, 3, 2}, labels,
        [](int a, int b) { return a == b; });
    EXPECT_EQ(classes, 3);
    EXPECT_EQ(labels[0], labels[2]);
    EXPECT_EQ(labels[1], labels[4]);
    EXPECT_NE(labels[0], labels[1]);
}

TEST(CoreCppTypes, ErrorCodesTraitsAndLoggingKeepTheirCppSurface) {
    static_assert(cv::traits::Type<cv::Vec3b>::value == mcpplibs::mwrap::opencv::macros::CV_8UC3);
    static_assert(cv::traits::Depth<float>::value == mcpplibs::mwrap::opencv::macros::CV_32F);
    static_assert(cv::ParamType<int>::type == cv::Param::INT);
    try {
        cv::error(cv::Error::StsBadArg, "module error surface", __func__, __FILE__, __LINE__);
        FAIL() << "cv::error must throw";
    } catch (const cv::Exception& error) {
        EXPECT_EQ(error.code, cv::Error::StsBadArg);
    }
    namespace logging = cv::utils::logging;
    const auto previous = logging::setLogLevel(logging::LOG_LEVEL_DEBUG);
    EXPECT_EQ(logging::getLogLevel(), logging::LOG_LEVEL_DEBUG);
    logging::setLogLevel(previous);
}

TEST(CoreCppTypes, QuaternionFreeFunctionsOperateThroughDirectImport) {
    const auto quarter_turn = cv::Quatd::createFromAngleAxis(
        mcpplibs::mwrap::opencv::macros::CV_PI / 2, cv::Vec3d(0, 0, 1));
    const auto inverted = cv::inv(quarter_turn).toRotMat3x3() * cv::Vec3d(1, 0, 0);
    EXPECT_NEAR(inverted[0], 0, 1e-12);
    EXPECT_NEAR(inverted[1], -1, 1e-12);
    const auto half_turn = cv::power(quarter_turn, 2.0).toRotMat3x3() * cv::Vec3d(1, 0, 0);
    EXPECT_NEAR(half_turn[0], -1, 1e-12);
    EXPECT_NEAR(half_turn[1], 0, 1e-12);
}

TEST(CoreCppTypes, DualQuaternionPreservesRigidTransform) {
    const auto pose = cv::DualQuatd::createFromAngleAxisTrans(
        0.0, cv::Vec3d(0, 0, 1), cv::Vec3d(1, 2, 3));
    const auto translated = pose.toAffine3() * cv::Point3d(4, 5, 6);
    EXPECT_NEAR(translated.x, 5, 1e-12);
    EXPECT_NEAR(translated.y, 7, 1e-12);
    EXPECT_NEAR(translated.z, 9, 1e-12);
}

TEST(CoreCppTypes, AsyncPromiseTransfersAMatrix) {
    cv::AsyncPromise promise;
    auto result = promise.getArrayResult();
    cv::Mat source(2, 3, mcpplibs::mwrap::opencv::macros::CV_32F, cv::Scalar(7));
    promise.setValue(source);
    cv::Mat destination;
    result.get(destination);
    ASSERT_EQ(destination.rows, 2);
    ASSERT_EQ(destination.cols, 3);
    EXPECT_FLOAT_EQ(destination.at<float>(1, 2), 7);
}

TEST(CoreCppTypes, FilesystemHelpersOperateOnATemporaryTree) {
    namespace fs = std::filesystem;
    const auto directory = fs::temp_directory_path() / ("opencv-mwrap-" +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    struct Cleanup {
        fs::path path;
        ~Cleanup() { std::error_code ec; fs::remove_all(path, ec); }
    } cleanup{directory};
    const auto nested = directory / "child";
    ASSERT_TRUE(cv::utils::fs::createDirectories(nested.string()));
    EXPECT_TRUE(cv::utils::fs::exists(nested.string()));
    EXPECT_TRUE(cv::utils::fs::isDirectory(nested.string()));
    cv::utils::fs::remove_all(directory.string());
    EXPECT_FALSE(cv::utils::fs::exists(directory.string()));
}

TEST(CoreCppTypes, TlsStorageSeparatesThreadValues) {
    cv::TLSData<int> storage;
    *storage.get() = 7;
    int child_value = 0;
    std::thread worker([&] {
        *storage.get() = 11;
        child_value = *storage.get();
    });
    worker.join();
    EXPECT_EQ(child_value, 11);
    EXPECT_EQ(*storage.get(), 7);
}

namespace {
class CallerParallelBackend final : public cv::parallel::ParallelForAPI {
    int threads_ = 2;
public:
    int calls = 0;
    void parallel_for(int tasks, FN_parallel_for_body_cb_t callback, void* data) override {
        ++calls;
        callback(0, tasks, data);
    }
    int getThreadNum() const override { return 0; }
    int getNumThreads() const override { return threads_; }
    int setNumThreads(int value) override { const int previous = threads_; threads_ = value; return previous; }
    const char* getName() const override { return "mwrap-caller"; }
};
class FillParallelBody final : public cv::ParallelLoopBody {
    std::vector<int>& values_;
public:
    explicit FillParallelBody(std::vector<int>& values) : values_(values) {}
    void operator()(const cv::Range& range) const override {
        for (int i = range.start; i < range.end; ++i) values_[i] = i + 1;
    }
};
}

TEST(CoreCppTypes, CallerCanSupplyTheParallelBackend) {
    const int previous_threads = cv::getNumThreads();
    cv::setNumThreads(2);
    auto backend = std::make_shared<CallerParallelBackend>();
    cv::parallel::setParallelForBackend(backend);
    struct Restore {
        int threads;
        ~Restore() {
            cv::parallel::setParallelForBackend(std::shared_ptr<cv::parallel::ParallelForAPI>{});
            cv::setNumThreads(threads);
        }
    } restore{previous_threads};
    std::vector<int> values(100, 0);
    cv::parallel_for_(cv::Range(0, 100), FillParallelBody(values), 2);
    EXPECT_GT(backend->calls, 0);
    for (int i = 0; i < 100; ++i) EXPECT_EQ(values[i], i + 1);
}

TEST(CoreCppTypes, CpuBuildExposesTheUpstreamDeviceQuerySurface) {
    EXPECT_EQ(cv::cuda::getCudaEnabledDeviceCount(), 0);
    static_assert(cv::cuda::FEATURE_SET_COMPUTE_20 == 20);
    static_assert(cv::ogl::TRIANGLES == 0x0004);
}

TEST(CoreCppTypes, StdAliasesAndImageMinMaxRetainTheirOverloads) {
    const double first = -0.0, second = 0.0;
    EXPECT_EQ(&cv::min(first, second), &first);
    EXPECT_EQ(&cv::max(first, second), &first);
    EXPECT_EQ(cv::min(std::string("alpha"), std::string("beta")), "alpha");
    EXPECT_EQ(cv::max(3, 5, std::greater<int>{}), 3);
    EXPECT_EQ(cv::min({3, 1, 2}), 1);
    cv::Mat a(2, 2, mcpplibs::mwrap::opencv::macros::CV_32F, cv::Scalar(6));
    cv::Mat b(2, 2, mcpplibs::mwrap::opencv::macros::CV_32F, cv::Scalar(3)), result;
    cv::min(a, b, result);
    EXPECT_FLOAT_EQ(result.at<float>(0, 0), 3);
    cv::max(a, b, result);
    EXPECT_FLOAT_EQ(result.at<float>(0, 0), 6);
    cv::UMat ua, ub, ur;
    a.copyTo(ua); b.copyTo(ub);
    cv::min(ua, ub, ur);
    EXPECT_FLOAT_EQ(ur.getMat(cv::ACCESS_READ).at<float>(0, 0), 3);
    cv::max(ua, ub, ur);
    EXPECT_FLOAT_EQ(ur.getMat(cv::ACCESS_READ).at<float>(0, 0), 6);
}

struct MwrapCustomResource { int* deletions; };
namespace cv {
template<> struct DefaultDeleter<MwrapCustomResource> {
    void operator()(MwrapCustomResource* resource) const {
        ++*resource->deletions;
        delete resource;
    }
};
}
TEST(CoreCppTypes, PtrHonorsTheUpstreamCustomDeleterTrait) {
    static_assert(cv::has_custom_delete<MwrapCustomResource>::value);
    int deletions = 0;
    {
        cv::Ptr<MwrapCustomResource> resource(new MwrapCustomResource{&deletions});
        ASSERT_FALSE(resource.empty());
        resource.release();
    }
    EXPECT_EQ(deletions, 1);
    EXPECT_EQ(*cv::makePtr<int>(7), 7);
}
