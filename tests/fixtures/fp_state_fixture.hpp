#pragma once
namespace fp_state_fixture {
using State = cv::details::FPDenormalsModeState;
inline bool same(const State& a, const State& b) {
    for (int i=0; i<16; ++i) if (a.reserved[i]!=b.reserved[i]) return false;
    return true;
}
inline State snapshot() {
    State result{};
    cv::details::saveFPDenormalsState(result);
    return result;
}
inline void require(bool value) {
    if (!value) throw std::runtime_error("SDK floating-point state was not restored");
}
inline void emit(const char* consumer) {
    static_assert(sizeof(State)==64);
    static_assert(std::is_same_v<decltype(&cv::details::saveFPDenormalsState),int (*)(State&)>);
    static_assert(std::is_same_v<decltype(&cv::details::restoreFPDenormalsState),bool (*)(const State&)>);
    static_assert(std::is_same_v<decltype(&cv::details::setFPDenormalsIgnoreHint),void (*)(bool,State&)>);
    State original{};
    const int slots=cv::details::saveFPDenormalsState(original);
    struct Restore {
        const State& state;
        int slots;
        ~Restore() { if(slots>0) cv::details::restoreFPDenormalsState(state); }
    } restore{original,slots};
    std::ostringstream out;
    out << sizeof(State) << ',' << slots << '\n';
    State saved{};
    cv::details::setFPDenormalsIgnoreHint(true,saved);
    out << same(original,saved) << ',' << same(original,snapshot()) << ',';
    const bool restored=cv::details::restoreFPDenormalsState(saved);
    require(same(original,snapshot()));
    out << restored << '\n';
    {
        cv::details::FPDenormalsIgnoreHintScope scope(true);
        out << same(original,snapshot()) << ',';
    }
    require(same(original,snapshot()));
    {
        cv::details::FPDenormalsIgnoreHintScope scope(false);
        out << same(original,snapshot()) << ',';
    }
    require(same(original,snapshot()));
    {
        cv::details::FPDenormalsIgnoreHintScope scope(original);
        require(same(original,snapshot()));
    }
    {
        cv::details::FPDenormalsIgnoreHintScopeNOOP scope(true), from_state(original);
        require(same(original,snapshot()));
    }
    out << same(original,snapshot()) << '\n';
#if defined(MCPP_FEATURE_VIDEOIO)
    // These SDK declarations are intentionally incomplete. Do not instantiate them.
    cv::IVideoCapture* capture=nullptr;
    cv::IVideoWriter* writer=nullptr;
    out << (capture==nullptr) << ',' << (writer==nullptr) << '\n';
#endif
    if (const char* directory=std::getenv("OPENCV_M_FP_OUTPUT")) {
        std::ofstream file(std::filesystem::path(directory)/(std::string(consumer)+".txt"),std::ios::binary);
        file << out.str();
        if (!file) throw std::runtime_error("FP state oracle output");
    }
}
}
