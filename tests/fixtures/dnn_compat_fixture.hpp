#pragma once
namespace dnn_compat_fixture {
template<class Function>
void unavailable(std::ostringstream& output, Function function) {
    try {
        function();
    } catch (const cv::Exception& error) {
        if (error.code != cv::Error::StsNotImplemented)
            throw std::runtime_error("unexpected SDK backend error");
        output << error.code << '\n';
        return;
    }
    throw std::runtime_error("unavailable backend silently accepted");
}

inline void emit(const char* consumer) {
    using namespace cv::dnn;
    static_assert(std::is_same_v<decltype(&getInferenceEngineBackendType),cv::String (*)()>);
    static_assert(std::is_same_v<decltype(&setInferenceEngineBackendType),cv::String (*)(const cv::String&)>);
    static_assert(std::is_same_v<decltype(&getInferenceEngineVPUType),cv::String (*)()>);
    static_assert(std::is_same_v<decltype(&getInferenceEngineCPUType),cv::String (*)()>);
    static_assert(std::is_same_v<decltype(&resetMyriadDevice),void (*)()>);
    static_assert(std::is_same_v<decltype(&releaseHDDLPlugin),void (*)()>);
    bool (*equal)(const Arg&,const Arg&)=&cv::dnn::operator==;
    bool (*qualified_equal)(const Arg&,const Arg&)=&cv::dnn::dnn5_v20260605::operator==;
    if (equal != qualified_equal) throw std::runtime_error("SDK ABI namespace equality identity");
    std::ostringstream output;
    for (int index : {0,1,-1,42}) {
        Arg left(index), same(index), other(index+1);
        if (!equal(left,same) || equal(left,other) || !(left==same) || left==other)
            throw std::runtime_error("SDK Arg equality changed");
        output << int(left) << ',' << left.empty() << ',' << equal(left,same) << '\n';
    }
    unavailable(output, [] { return getInferenceEngineBackendType(); });
    unavailable(output, [] { return setInferenceEngineBackendType("NGRAPH"); });
    unavailable(output, [] { return getInferenceEngineVPUType(); });
    unavailable(output, [] { return getInferenceEngineCPUType(); });
    // These are original SDK no-ops in this build without Inference Engine.
    resetMyriadDevice();
    releaseHDDLPlugin();
    output << "SDK unavailable-backend and cleanup contracts\n";
    if (const char* directory=std::getenv("OPENCV_M_DNN_COMPAT_OUTPUT")) {
        std::ofstream file(std::filesystem::path(directory)/(std::string(consumer)+".txt"),std::ios::binary);
        file << output.str();
        if (!file) throw std::runtime_error("DNN compatibility oracle output");
    }
}
}
