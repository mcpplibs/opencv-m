// Affine free operators are static in upstream affine.hpp. Mirror their
// implementation in the existing wrapper replacement namespace; class members
// and aliases retain their upstream identity. Never export a TU-local using.
export namespace cv::opencv_m_repl {
template<typename T> requires opencv_m_pick<T>
inline Affine3<T> operator*(const Affine3<T>& a, const Affine3<T>& b) {
    return b.concatenate(a);
}

template<typename T, typename V> requires opencv_m_pick<T> && requires(V v) { v.x; v.y; v.z; }
inline V operator*(const Affine3<T>& affine, const V& v) {
    const auto& m = affine.matrix;
    V result;
    result.x = m.val[0] * v.x + m.val[1] * v.y + m.val[2] * v.z + m.val[3];
    result.y = m.val[4] * v.x + m.val[5] * v.y + m.val[6] * v.z + m.val[7];
    result.z = m.val[8] * v.x + m.val[9] * v.y + m.val[10] * v.z + m.val[11];
    return result;
}

template<typename T> requires (__is_same(T, float) || __is_same(T, double))
inline Vec<T, 3> operator*(const Affine3<T>& affine, const Vec<T, 3>& v) {
    const auto& m = affine.matrix;
    Vec<T, 3> result;
    for (int row = 0; row < 3; ++row)
        result[row] = m.val[4 * row] * v[0] + m.val[4 * row + 1] * v[1] +
                      m.val[4 * row + 2] * v[2] + m.val[4 * row + 3];
    return result;
}
}
