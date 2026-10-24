#pragma once
namespace simd_legacy_fixture {
using V = cv::v_float32x4;
inline void dump(std::ostringstream& out, const V& value) {
    std::array<unsigned,4> bits{};
    cv::v_store(bits.data(), cv::v_reinterpret_as_u32(value));
    for (auto bit : bits) out << bit << ',';
    out << '\n';
}
template<class F> void compound(std::ostringstream& out, const V& a, const V& b, F apply) {
    V copy = a;
    V& reference = apply(copy,b);
    if (&reference != &copy) throw std::runtime_error("legacy compound reference identity");
    dump(out, copy);
}
inline void emit(const char* consumer) {
    using namespace cv::hal;
    const V a = cv::v_setall_f32(6), b = cv::v_setall_f32(2);
    std::ostringstream out;
    dump(out, cv::hal::operator+(a,b));
    dump(out, cv::hal::operator-(a,b));
    dump(out, cv::hal::operator*(a,b));
    dump(out, cv::hal::operator/(a,b));
    dump(out, cv::hal::operator&(a,b));
    dump(out, cv::hal::operator|(a,b));
    dump(out, cv::hal::operator^(a,b));
    dump(out, cv::hal::operator==(a,b));
    dump(out, cv::hal::operator!=(a,b));
    dump(out, cv::hal::operator<(a,b));
    dump(out, cv::hal::operator>(a,b));
    dump(out, cv::hal::operator<=(a,b));
    dump(out, cv::hal::operator>=(a,b));
#if defined(MCPP_FEATURE_SIMD_EMULATED)
    // The original emulator declares v_not<float,N> without a definition.
    // Exercise the defined integer overload; retain the SDK's floating failure.
    dump(out, cv::v_reinterpret_as_f32(cv::hal::operator~(cv::v_reinterpret_as_u32(a))));
#else
    dump(out, cv::hal::operator~(a));
#endif
    compound(out,a,b,[](auto& x,const auto& y)->auto& { return cv::hal::operator+=(x,y); });
    compound(out,a,b,[](auto& x,const auto& y)->auto& { return cv::hal::operator-=(x,y); });
    compound(out,a,b,[](auto& x,const auto& y)->auto& { return cv::hal::operator*=(x,y); });
    compound(out,a,b,[](auto& x,const auto& y)->auto& { return cv::hal::operator/=(x,y); });
    compound(out,a,b,[](auto& x,const auto& y)->auto& { return cv::hal::operator&=(x,y); });
    compound(out,a,b,[](auto& x,const auto& y)->auto& { return cv::hal::operator|=(x,y); });
    compound(out,a,b,[](auto& x,const auto& y)->auto& { return cv::hal::operator^=(x,y); });
    // Normal operator syntax and explicit template/function-pointer selection.
    dump(out, a+b);
    using Add = V (*)(const V&,const V&);
    const auto add = static_cast<Add>(&cv::hal::operator+<V>);
    dump(out, add(a,b));
    std::array<uchar,16> bytes{};
    const auto high=cv::v_setall_u8(250), ten=cv::v_setall_u8(10);
    cv::v_store(bytes.data(), high+ten);
    for (auto value : bytes) {
        if (value != 255) throw std::runtime_error("legacy byte saturation");
        out << +value << ',';
    }
    out << '\n';
    if (const char* directory=std::getenv("OPENCV_M_SIMD_OUTPUT")) {
        std::ofstream file(std::filesystem::path(directory)/(std::string(consumer)+"-legacy.txt"),std::ios::binary);
        file << out.str();
        if (!file) throw std::runtime_error("legacy SIMD oracle output");
    }
}
}
