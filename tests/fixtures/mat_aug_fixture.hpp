#pragma once
namespace mat_aug_fixture {
template<class T, class Left, class Right, class Operation>
void record(std::ostringstream& out, const Left& initial, const Right& right, Operation operation) {
    for (bool constant : {false, true}) {
        Left value(initial.clone());
        if (constant) {
            const Left& left = value;
            static_assert(std::is_same_v<decltype(operation(left, right)), const Left&>);
            if (&operation(left, right) != &left) throw std::runtime_error("const assignment identity");
        } else {
            static_assert(std::is_same_v<decltype(operation(value, right)), Left&>);
            if (&operation(value, right) != &value) throw std::runtime_error("assignment identity");
        }
        for (int row = 0; row < 2; ++row)
            for (int col = 0; col < 2; ++col) out << double(value.template at<T>(row, col)) << ',';
        out << '\n';
    }
}

template<class T, class Operation>
void family(std::ostringstream& out, Operation operation) {
    cv::Mat_<T> left(2, 2), right(2, 2);
    left(0,0)=8; left(0,1)=4; left(1,0)=2; left(1,1)=6;
    right(0,0)=2; right(0,1)=1; right(1,0)=1; right(1,1)=2;
    const cv::Mat untyped_left=left, untyped_right=right;
    const cv::Matx<T,2,2> fixed(2,1,1,2);
    record<T>(out,untyped_left,untyped_right,operation);
    record<T>(out,left,untyped_right,operation);
    record<T>(out,left,right,operation);
    record<T>(out,untyped_left,fixed,operation);
    record<T>(out,left,fixed,operation);
}

inline void emit(const char* consumer) {
    std::ostringstream out;
#define MWRAP_AUG(OP, TYPE, RIGHT) \
    { \
        const auto operation=[](auto& a,const auto& b)->decltype(auto) { return cv::operator OP(a,b); }; \
        family<TYPE>(out,operation); \
        cv::Mat_<TYPE> typed(2,2,TYPE(8)); \
        const cv::Mat untyped=typed; \
        record<TYPE>(out,untyped,RIGHT,operation); \
        record<TYPE>(out,typed,RIGHT,operation); \
    }
    MWRAP_AUG(+=, float, cv::Scalar(2))
    MWRAP_AUG(-=, float, cv::Scalar(2))
    MWRAP_AUG(*=, float, 2.0)
    MWRAP_AUG(/=, float, 2.0)
    MWRAP_AUG(&=, uchar, cv::Scalar(3))
    MWRAP_AUG(|=, uchar, cv::Scalar(3))
    MWRAP_AUG(^=, uchar, cv::Scalar(3))
#undef MWRAP_AUG
    using Typed=cv::Mat_<uchar>;
    using Fixed=cv::Matx<uchar,2,2>;
    Typed& (*typed)(Typed&,const Typed&)=&cv::operator^=<uchar>;
    const Typed& (*constant)(const Typed&,const Fixed&)=&cv::operator^=<uchar,2,2>;
    cv::Mat& (*plain)(cv::Mat&,const cv::Scalar&)=&cv::operator^=;
    Typed a(2,2,uchar(8)), b(2,2,uchar(3));
    if (&typed(a,b)!=&a || a(0,0)!=11) throw std::runtime_error("typed explicit assignment");
    const Fixed fixed(1,1,1,1);
    const Typed& c=a;
    if (&constant(c,fixed)!=&c || a(0,0)!=10) throw std::runtime_error("fixed explicit assignment");
    cv::Mat plain_a=a;
    if (&plain(plain_a,cv::Scalar(2))!=&plain_a || a(0,0)!=8) throw std::runtime_error("plain assignment pointer");
    out << "explicit-signatures-and-identity\n";
    if (const char* directory=std::getenv("OPENCV_M_MAT_AUG_OUTPUT")) {
        std::ofstream file(std::filesystem::path(directory)/(std::string(consumer)+".txt"),std::ios::binary);
        file << out.str();
        if (!file) throw std::runtime_error("Mat assignment oracle output");
    }
}
}
