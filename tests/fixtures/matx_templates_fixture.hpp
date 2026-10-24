#pragma once
// Both consumers instantiate the same public SDK expressions independently.
// Comma initializers are deprecated upstream; testing their compatibility is intentional.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
namespace matx_templates_fixture {
template<class T, unsigned N> void values(std::ostringstream& out, const T (&data)[N]) {
    for (auto value : data) out << +value << ',';
    out << '\n';
}
template<int Mode> void matrix_initializer(std::ostringstream& out) {
    cv::Matx<float, 2, 3> m;
    auto first = [&] {
        if constexpr (Mode == 0) return cv::operator<<(m, 1.75);
        else if constexpr (Mode == 1) return cv::operator<<<float>(m, 1.75);
        else if constexpr (Mode == 2) return cv::operator<<<float, int>(m, 1.75);
        else if constexpr (Mode == 3) return cv::operator<<<float, int, 2>(m, 1.75);
        else return cv::operator<<<float, int, 2, 3>(m, 1.75);
    }();
    (first, 2, 3, 4, 5, 6);
    values(out, m.val);
}
template<int Mode> void vector_initializer(std::ostringstream& out) {
    cv::Vec<float, 3> v;
    auto first = [&] {
        if constexpr (Mode == 0) return cv::operator<<(v, 1.75);
        else if constexpr (Mode == 1) return cv::operator<<<float>(v, 1.75);
        else if constexpr (Mode == 2) return cv::operator<<<float, int>(v, 1.75);
        else return cv::operator<<<float, int, 3>(v, 1.75);
    }();
    (first, 2, 3);
    values(out, v.val);
}
template<int Mode> void matrix_augmentation(std::ostringstream& out) {
    cv::Matx<uchar, 2, 2> a(250, 3, 8, 10);
    const cv::Matx<float, 2, 2> b(10.75f, -7.25f, 2.5f, 5.5f);
    if constexpr (Mode == 0) { a += b; a -= b; }
    else if constexpr (Mode == 1) { cv::operator+=<uchar>(a,b); cv::operator-=<uchar>(a,b); }
    else if constexpr (Mode == 2) { cv::operator+=<uchar,float>(a,b); cv::operator-=<uchar,float>(a,b); }
    else if constexpr (Mode == 3) { cv::operator+=<uchar,float,2>(a,b); cv::operator-=<uchar,float,2>(a,b); }
    else { cv::operator+=<uchar,float,2,2>(a,b); cv::operator-=<uchar,float,2,2>(a,b); }
    values(out, a.val);
}
template<int Mode> void vector_augmentation(std::ostringstream& out) {
    cv::Vec<uchar, 3> a(250, 3, 8);
    const cv::Vec<float, 3> b(10.75f, -7.25f, 2.5f);
    if constexpr (Mode == 0) { a += b; a -= b; }
    else if constexpr (Mode == 1) { cv::operator+=<uchar>(a,b); cv::operator-=<uchar>(a,b); }
    else if constexpr (Mode == 2) { cv::operator+=<uchar,float>(a,b); cv::operator-=<uchar,float>(a,b); }
    else { cv::operator+=<uchar,float,3>(a,b); cv::operator-=<uchar,float,3>(a,b); }
    values(out, a.val);
}
template<int Mode> void matrix_product(std::ostringstream& out) {
    const cv::Matx<float, 2, 3> a(1, 2, 3, 4, 5, 6);
    const cv::Matx<float, 3, 2> b(7, 8, 9, 10, 11, 12);
    auto product = [&] {
        if constexpr (Mode == 0) return a*b;
        else if constexpr (Mode == 1) return cv::operator*<float>(a,b);
        else if constexpr (Mode == 2) return cv::operator*<float,2>(a,b);
        else if constexpr (Mode == 3) return cv::operator*<float,2,2>(a,b);
        else return cv::operator*<float,2,2,3>(a,b);
    }();
    static_assert(std::is_same_v<decltype(product), cv::Matx22f>);
    values(out, product.val);
}
inline void emit(const char* consumer) {
    std::ostringstream out;
    out << std::setprecision(17);
    const cv::Matx33f a(4,2,1,0,5,3,2,1,6);
    using Determinant = double (*)(const cv::Matx33f&);
    const auto determinant = static_cast<Determinant>(&cv::determinant<float,3>);
    out << cv::determinant(a) << ',' << cv::determinant<float>(a) << ','
        << cv::determinant<float,3>(a) << ',' << determinant(a) << '\n';
    const cv::Matx44d large(2,1,0,0,0,3,1,0,0,0,4,1,1,0,0,5);
    out << cv::determinant<double,4>(large) << '\n';
    matrix_initializer<0>(out); matrix_initializer<1>(out); matrix_initializer<2>(out);
    matrix_initializer<3>(out); matrix_initializer<4>(out);
    vector_initializer<0>(out); vector_initializer<1>(out); vector_initializer<2>(out); vector_initializer<3>(out);
    matrix_augmentation<0>(out); matrix_augmentation<1>(out); matrix_augmentation<2>(out);
    matrix_augmentation<3>(out); matrix_augmentation<4>(out);
    vector_augmentation<0>(out); vector_augmentation<1>(out); vector_augmentation<2>(out); vector_augmentation<3>(out);
    matrix_product<0>(out); matrix_product<1>(out); matrix_product<2>(out);
    matrix_product<3>(out); matrix_product<4>(out);
    // Derived-to-base deduction and exact SDK reference-return types.
    cv::Matx<float, 3, 1> column(1,2,3);
    cv::Vec3f v(4,5,6);
    static_assert(std::is_same_v<decltype(column += v), cv::Matx<float,3,1>&>);
    column += v;
    static_assert(std::is_same_v<decltype(v += column), cv::Matx<float,3,1>&>);
    v += column;
    values(out, column.val); values(out, v.val);
    using Augment = cv::Matx<uchar,2,2>& (*)(cv::Matx<uchar,2,2>&, const cv::Matx<float,2,2>&);
    const auto augment = static_cast<Augment>(&cv::operator+=<uchar,float,2,2>);
    cv::Matx<uchar,2,2> bytes = cv::Matx<uchar,2,2>::all(250);
    augment(bytes, cv::Matx<float,2,2>::all(10.75f));
    values(out, bytes.val);
    struct Derived : cv::Matx33f { using cv::Matx33f::Matx; };
    const Derived derived(4,2,1,0,5,3,2,1,6);
    out << cv::determinant(derived) << '\n';
    using Product = cv::Matx22f (*)(const cv::Matx<float,2,3>&, const cv::Matx<float,3,2>&);
    const auto product = static_cast<Product>(&cv::operator*<float,2,2,3>);
    values(out, product(cv::Matx<float,2,3>::all(1),cv::Matx<float,3,2>::all(2)).val);
    if (const char* directory = std::getenv("OPENCV_M_MATX_OUTPUT")) {
        std::ofstream file(std::filesystem::path(directory) / (std::string(consumer) + ".txt"), std::ios::binary);
        file << out.str();
        if (!file) throw std::runtime_error("Matx template oracle output");
    }
}
}
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif
