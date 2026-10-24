#include <gtest/gtest.h>
#if defined(MCPP_FEATURE_DNN_ONNXRUNTIME)
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include "fixtures/ort_model.hpp"

import mcpplibs.mwrap.onnxruntime;
import mcpplibs.mwrap.opencv;

namespace fs = std::filesystem;

class OrtTestFiles {
    std::string previous;
    bool had_previous;
public:
    fs::path directory;
    OrtTestFiles() {
        const auto* old = std::getenv("OPENCV_TEMP_PATH");
        had_previous = old != nullptr;
        if (old) previous = old;
        directory = fs::temp_directory_path() / ("mwrap-opencv-ort-" +
            std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        fs::create_directories(directory);
        const auto path = directory.string();
#ifdef _WIN32
        _putenv_s("OPENCV_TEMP_PATH", path.c_str());
#else
        setenv("OPENCV_TEMP_PATH", path.c_str(), 1);
#endif
    }
    ~OrtTestFiles() {
#ifdef _WIN32
        _putenv_s("OPENCV_TEMP_PATH", had_previous ? previous.c_str() : "");
#else
        if (had_previous) setenv("OPENCV_TEMP_PATH", previous.c_str(), 1);
        else unsetenv("OPENCV_TEMP_PATH");
#endif
        std::error_code error;
        fs::remove_all(directory, error);
    }
};

TEST(DnnOnnxruntime, ExecutesOrtCpuAndProducesItsSessionProfile) {
    OrtTestFiles files;
    const auto model = files.directory / "add.onnx";
    {
        std::ofstream out(model, std::ios::binary);
        out.write(reinterpret_cast<const char*>(opencv_ort_model), sizeof(opencv_ort_model));
    }
    ASSERT_EQ(std::string(Ort::GetVersionString()), "1.25.1");
    auto net = cv::dnn::readNetFromONNX(model.string(), cv::dnn::ENGINE_ORT);
    ASSERT_FALSE(net.empty());
    net.setProfilingMode(cv::dnn::DNN_PROFILE_DETAILED);
    net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    cv::Mat input(1, 3, mcpplibs::mwrap::opencv::macros::CV_32F);
    for (int i = 0; i < 3; ++i) input.at<float>(0, i) = float(i + 1);
    net.setInput(input, "x");
    const auto output = net.forward("y");
    ASSERT_EQ(output.total(), 3u);
    for (int i = 0; i < 3; ++i) EXPECT_FLOAT_EQ(output.ptr<float>()[i], float(2 * (i + 1)));
    std::vector<std::string> names, times, counts;
    net.getPerfProfile(names, times, counts); // closes the real ORT profiler
    bool executed_cpu = false;
    for (const auto& entry : fs::directory_iterator(files.directory)) {
        if (entry.path().extension() != ".json") continue;
        std::ifstream in(entry.path());
        const std::string json((std::istreambuf_iterator<char>(in)), {});
        if (json.find("model_run") != std::string::npos &&
            json.find("CPUExecutionProvider") != std::string::npos)
            executed_cpu = true;
    }
    EXPECT_TRUE(executed_cpu) << "No ORT CPU session profile; native OpenCV fallback must not pass";
}
#endif
