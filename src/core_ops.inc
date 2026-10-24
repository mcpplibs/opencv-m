// core_ops.inc — hand-maintained replacement surface for upstream entities
// that CANNOT cross a module boundary because OpenCV declares them
// `static inline` (internal linkage): the namespace-scope operator templates
// for the everyday value types, and saturate_cast which they depend on.
//
// SDK-generated value operators live in type_ops.inc; scalar conversions
// live in saturate_ops.inc. This file owns the ordinary cvRound bridges,
// shared constraint and Mat/MatExpr external-function forwards. Matx/Vec
// algebra lives in matx_ops.inc.
//
// MIXED-TU STRATEGY (v0.0.3, gcc16-verified):
// - TEMPLATE replacements carry the trivially-true `opencv_m_pick`
//   constraint on an IDENTICAL upstream signature: in a mixed TU (textual
//   <opencv2/…> include + import) the two candidates tie in partial
//   ordering and the MORE CONSTRAINED one — ours — wins deterministically.
//   That also sidesteps a gcc16 linkage trap: the header's static-inline
//   instantiation home is picked arbitrarily between the importer TU and
//   the module GMF (undefined refs when it lands module-side); our
//   entities are module-linkage and always defined.
// - NON-TEMPLATE upstream surfaces (Range ops, Mat/MatExpr CV_EXPORTS
//   operators) are wrapped as CONSTRAINED TEMPLATES instead: upstream's
//   plain functions win in mixed TUs (Range's static-inline non-templates
//   emit reliably in the including TU; Mat's are external in the compat
//   lib), and pure-import TUs use the wrappers.
// Net: every operator works in BOTH worlds — the v0.0.2 "prefer member
// forms in mixed TUs" caveat is gone.

// Non-inline module-private bridges preserve the SDK's exact platform rounding
// without exposing its TU-local helpers through exported conversion templates.
namespace opencv_m_scalar_bridge {
int round32(float value) { return ::cvRound(value); }
int round32(double value) { return ::cvRound(value); }
}

export namespace cv {
// inline namespace: distinct entities from the upstream TU-local ones (no
// in-module redefinition), yet reachable as cv::… via qualified lookup and
// ADL on the importer side, where the upstream internals are invisible.
inline namespace opencv_m_repl {

// trivially-true constraint: makes each template replacement MORE
// CONSTRAINED than upstream's identical-signature one (see header comment)
template<class> inline constexpr bool opencv_m_pick = true;

#include "saturate_ops.inc"

// Point/Point3/Size/Rect/Range/Scalar/Complex operators are generated
// from the pinned SDK in type_ops.inc, after Matx operators are available.

} // inline namespace opencv_m_repl
} // namespace cv

// ── Mat / MatExpr operator delegation ───────────────────────────────────
// Upstream declares these CV_EXPORTS (external) — but the plain
// `using cv::operator+;` route would also drag the TU-local template
// overload sets into importers and collide with the replacements above.
// Instead: capture each upstream overload by exact function-pointer type
// (unique even though calls would be ambiguous), then re-expose it as a
// delegating operator inside the exported inline namespace — as a
// CONSTRAINED TEMPLATE, so in mixed TUs upstream's plain functions win
// (no tie, no ambiguity) while pure-import TUs use these. `double`
// parameters are loosened to any arithmetic type (callers write `m > 5`);
// the delegation converts exactly like upstream's double parameter would.
namespace opencv_m_up {
inline constexpr auto up_ne_sh_sh = static_cast<bool (*)(const cv::MatShape&, const cv::MatShape&)>(&cv::operator !=);
inline constexpr auto up_eq_sh_sh = static_cast<bool (*)(const cv::MatShape&, const cv::MatShape&)>(&cv::operator ==);
inline constexpr auto up_ne_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator !=);
inline constexpr auto up_and_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator &);
inline constexpr auto up_mul_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator *);
inline constexpr auto up_add_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator +);
inline constexpr auto up_sub_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator -);
inline constexpr auto up_div_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator /);
inline constexpr auto up_lt_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator <);
inline constexpr auto up_le_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator <=);
inline constexpr auto up_eq_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator ==);
inline constexpr auto up_gt_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator >);
inline constexpr auto up_ge_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator >=);
inline constexpr auto up_xor_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator ^);
inline constexpr auto up_or_m_m = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Mat&)>(&cv::operator |);
inline constexpr auto up_and_m_s = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Scalar&)>(&cv::operator &);
inline constexpr auto up_add_m_s = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Scalar&)>(&cv::operator +);
inline constexpr auto up_sub_m_s = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Scalar&)>(&cv::operator -);
inline constexpr auto up_xor_m_s = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Scalar&)>(&cv::operator ^);
inline constexpr auto up_or_m_s = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::Scalar&)>(&cv::operator |);
inline constexpr auto up_ne_m_d = static_cast<cv::MatExpr (*)(const cv::Mat&, double)>(&cv::operator !=);
inline constexpr auto up_mul_m_d = static_cast<cv::MatExpr (*)(const cv::Mat&, double)>(&cv::operator *);
inline constexpr auto up_div_m_d = static_cast<cv::MatExpr (*)(const cv::Mat&, double)>(&cv::operator /);
inline constexpr auto up_lt_m_d = static_cast<cv::MatExpr (*)(const cv::Mat&, double)>(&cv::operator <);
inline constexpr auto up_le_m_d = static_cast<cv::MatExpr (*)(const cv::Mat&, double)>(&cv::operator <=);
inline constexpr auto up_eq_m_d = static_cast<cv::MatExpr (*)(const cv::Mat&, double)>(&cv::operator ==);
inline constexpr auto up_gt_m_d = static_cast<cv::MatExpr (*)(const cv::Mat&, double)>(&cv::operator >);
inline constexpr auto up_ge_m_d = static_cast<cv::MatExpr (*)(const cv::Mat&, double)>(&cv::operator >=);
inline constexpr auto up_sub_e = static_cast<cv::MatExpr (*)(const cv::MatExpr&)>(&cv::operator -);
inline constexpr auto up_mul_e_e = static_cast<cv::MatExpr (*)(const cv::MatExpr&, const cv::MatExpr&)>(&cv::operator *);
inline constexpr auto up_add_e_e = static_cast<cv::MatExpr (*)(const cv::MatExpr&, const cv::MatExpr&)>(&cv::operator +);
inline constexpr auto up_sub_e_e = static_cast<cv::MatExpr (*)(const cv::MatExpr&, const cv::MatExpr&)>(&cv::operator -);
inline constexpr auto up_div_e_e = static_cast<cv::MatExpr (*)(const cv::MatExpr&, const cv::MatExpr&)>(&cv::operator /);
inline constexpr auto up_mul_e_m = static_cast<cv::MatExpr (*)(const cv::MatExpr&, const cv::Mat&)>(&cv::operator *);
inline constexpr auto up_add_e_m = static_cast<cv::MatExpr (*)(const cv::MatExpr&, const cv::Mat&)>(&cv::operator +);
inline constexpr auto up_sub_e_m = static_cast<cv::MatExpr (*)(const cv::MatExpr&, const cv::Mat&)>(&cv::operator -);
inline constexpr auto up_div_e_m = static_cast<cv::MatExpr (*)(const cv::MatExpr&, const cv::Mat&)>(&cv::operator /);
inline constexpr auto up_add_e_s = static_cast<cv::MatExpr (*)(const cv::MatExpr&, const cv::Scalar&)>(&cv::operator +);
inline constexpr auto up_sub_e_s = static_cast<cv::MatExpr (*)(const cv::MatExpr&, const cv::Scalar&)>(&cv::operator -);
inline constexpr auto up_mul_e_d = static_cast<cv::MatExpr (*)(const cv::MatExpr&, double)>(&cv::operator *);
inline constexpr auto up_div_e_d = static_cast<cv::MatExpr (*)(const cv::MatExpr&, double)>(&cv::operator /);
inline constexpr auto up_sub_m = static_cast<cv::MatExpr (*)(const cv::Mat&)>(&cv::operator -);
inline constexpr auto up_not_m = static_cast<cv::MatExpr (*)(const cv::Mat&)>(&cv::operator ~);
inline constexpr auto up_mul_m_e = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::MatExpr&)>(&cv::operator *);
inline constexpr auto up_add_m_e = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::MatExpr&)>(&cv::operator +);
inline constexpr auto up_sub_m_e = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::MatExpr&)>(&cv::operator -);
inline constexpr auto up_div_m_e = static_cast<cv::MatExpr (*)(const cv::Mat&, const cv::MatExpr&)>(&cv::operator /);
inline constexpr auto up_and_s_m = static_cast<cv::MatExpr (*)(const cv::Scalar&, const cv::Mat&)>(&cv::operator &);
inline constexpr auto up_add_s_m = static_cast<cv::MatExpr (*)(const cv::Scalar&, const cv::Mat&)>(&cv::operator +);
inline constexpr auto up_sub_s_m = static_cast<cv::MatExpr (*)(const cv::Scalar&, const cv::Mat&)>(&cv::operator -);
inline constexpr auto up_xor_s_m = static_cast<cv::MatExpr (*)(const cv::Scalar&, const cv::Mat&)>(&cv::operator ^);
inline constexpr auto up_or_s_m = static_cast<cv::MatExpr (*)(const cv::Scalar&, const cv::Mat&)>(&cv::operator |);
inline constexpr auto up_add_s_e = static_cast<cv::MatExpr (*)(const cv::Scalar&, const cv::MatExpr&)>(&cv::operator +);
inline constexpr auto up_sub_s_e = static_cast<cv::MatExpr (*)(const cv::Scalar&, const cv::MatExpr&)>(&cv::operator -);
inline constexpr auto up_ne_d_m = static_cast<cv::MatExpr (*)(double, const cv::Mat&)>(&cv::operator !=);
inline constexpr auto up_mul_d_m = static_cast<cv::MatExpr (*)(double, const cv::Mat&)>(&cv::operator *);
inline constexpr auto up_div_d_m = static_cast<cv::MatExpr (*)(double, const cv::Mat&)>(&cv::operator /);
inline constexpr auto up_lt_d_m = static_cast<cv::MatExpr (*)(double, const cv::Mat&)>(&cv::operator <);
inline constexpr auto up_le_d_m = static_cast<cv::MatExpr (*)(double, const cv::Mat&)>(&cv::operator <=);
inline constexpr auto up_eq_d_m = static_cast<cv::MatExpr (*)(double, const cv::Mat&)>(&cv::operator ==);
inline constexpr auto up_gt_d_m = static_cast<cv::MatExpr (*)(double, const cv::Mat&)>(&cv::operator >);
inline constexpr auto up_ge_d_m = static_cast<cv::MatExpr (*)(double, const cv::Mat&)>(&cv::operator >=);
inline constexpr auto up_mul_d_e = static_cast<cv::MatExpr (*)(double, const cv::MatExpr&)>(&cv::operator *);
inline constexpr auto up_div_d_e = static_cast<cv::MatExpr (*)(double, const cv::MatExpr&)>(&cv::operator /);
} // namespace opencv_m_up

export namespace cv {
inline namespace opencv_m_repl {
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatShape) && __is_same(_A1, cv::MatShape))
inline bool operator != (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_ne_sh_sh(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatShape) && __is_same(_A1, cv::MatShape))
inline bool operator == (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_eq_sh_sh(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator != (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_ne_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator & (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_and_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator * (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_mul_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator + (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_add_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator - (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_sub_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator / (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_div_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator < (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_lt_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator <= (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_le_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator == (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_eq_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator > (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_gt_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator >= (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_ge_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator ^ (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_xor_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator | (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_or_m_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Scalar))
inline cv::MatExpr operator & (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_and_m_s(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Scalar))
inline cv::MatExpr operator + (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_add_m_s(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Scalar))
inline cv::MatExpr operator - (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_sub_m_s(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Scalar))
inline cv::MatExpr operator ^ (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_xor_m_s(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::Scalar))
inline cv::MatExpr operator | (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_or_m_s(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && std::is_arithmetic_v<_A1>)
inline cv::MatExpr operator != (const _A0& a0, _A1 a1) { return ::opencv_m_up::up_ne_m_d(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && std::is_arithmetic_v<_A1>)
inline cv::MatExpr operator * (const _A0& a0, _A1 a1) { return ::opencv_m_up::up_mul_m_d(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && std::is_arithmetic_v<_A1>)
inline cv::MatExpr operator / (const _A0& a0, _A1 a1) { return ::opencv_m_up::up_div_m_d(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && std::is_arithmetic_v<_A1>)
inline cv::MatExpr operator < (const _A0& a0, _A1 a1) { return ::opencv_m_up::up_lt_m_d(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && std::is_arithmetic_v<_A1>)
inline cv::MatExpr operator <= (const _A0& a0, _A1 a1) { return ::opencv_m_up::up_le_m_d(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && std::is_arithmetic_v<_A1>)
inline cv::MatExpr operator == (const _A0& a0, _A1 a1) { return ::opencv_m_up::up_eq_m_d(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && std::is_arithmetic_v<_A1>)
inline cv::MatExpr operator > (const _A0& a0, _A1 a1) { return ::opencv_m_up::up_gt_m_d(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && std::is_arithmetic_v<_A1>)
inline cv::MatExpr operator >= (const _A0& a0, _A1 a1) { return ::opencv_m_up::up_ge_m_d(a0, a1); }
template<class _A0> requires (__is_same(_A0, cv::MatExpr))
inline cv::MatExpr operator - (const _A0& a0) { return ::opencv_m_up::up_sub_e(a0); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator * (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_mul_e_e(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator + (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_add_e_e(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator - (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_sub_e_e(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator / (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_div_e_e(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator * (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_mul_e_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator + (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_add_e_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator - (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_sub_e_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator / (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_div_e_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && __is_same(_A1, cv::Scalar))
inline cv::MatExpr operator + (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_add_e_s(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && __is_same(_A1, cv::Scalar))
inline cv::MatExpr operator - (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_sub_e_s(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && std::is_arithmetic_v<_A1>)
inline cv::MatExpr operator * (const _A0& a0, _A1 a1) { return ::opencv_m_up::up_mul_e_d(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::MatExpr) && std::is_arithmetic_v<_A1>)
inline cv::MatExpr operator / (const _A0& a0, _A1 a1) { return ::opencv_m_up::up_div_e_d(a0, a1); }
template<class _A0> requires (__is_same(_A0, cv::Mat))
inline cv::MatExpr operator - (const _A0& a0) { return ::opencv_m_up::up_sub_m(a0); }
template<class _A0> requires (__is_same(_A0, cv::Mat))
inline cv::MatExpr operator ~ (const _A0& a0) { return ::opencv_m_up::up_not_m(a0); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator * (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_mul_m_e(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator + (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_add_m_e(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator - (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_sub_m_e(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Mat) && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator / (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_div_m_e(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Scalar) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator & (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_and_s_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Scalar) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator + (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_add_s_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Scalar) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator - (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_sub_s_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Scalar) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator ^ (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_xor_s_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Scalar) && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator | (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_or_s_m(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Scalar) && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator + (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_add_s_e(a0, a1); }
template<class _A0, class _A1> requires (__is_same(_A0, cv::Scalar) && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator - (const _A0& a0, const _A1& a1) { return ::opencv_m_up::up_sub_s_e(a0, a1); }
template<class _A0, class _A1> requires (std::is_arithmetic_v<_A0> && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator != (_A0 a0, const _A1& a1) { return ::opencv_m_up::up_ne_d_m(a0, a1); }
template<class _A0, class _A1> requires (std::is_arithmetic_v<_A0> && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator * (_A0 a0, const _A1& a1) { return ::opencv_m_up::up_mul_d_m(a0, a1); }
template<class _A0, class _A1> requires (std::is_arithmetic_v<_A0> && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator / (_A0 a0, const _A1& a1) { return ::opencv_m_up::up_div_d_m(a0, a1); }
template<class _A0, class _A1> requires (std::is_arithmetic_v<_A0> && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator < (_A0 a0, const _A1& a1) { return ::opencv_m_up::up_lt_d_m(a0, a1); }
template<class _A0, class _A1> requires (std::is_arithmetic_v<_A0> && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator <= (_A0 a0, const _A1& a1) { return ::opencv_m_up::up_le_d_m(a0, a1); }
template<class _A0, class _A1> requires (std::is_arithmetic_v<_A0> && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator == (_A0 a0, const _A1& a1) { return ::opencv_m_up::up_eq_d_m(a0, a1); }
template<class _A0, class _A1> requires (std::is_arithmetic_v<_A0> && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator > (_A0 a0, const _A1& a1) { return ::opencv_m_up::up_gt_d_m(a0, a1); }
template<class _A0, class _A1> requires (std::is_arithmetic_v<_A0> && __is_same(_A1, cv::Mat))
inline cv::MatExpr operator >= (_A0 a0, const _A1& a1) { return ::opencv_m_up::up_ge_d_m(a0, a1); }
template<class _A0, class _A1> requires (std::is_arithmetic_v<_A0> && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator * (_A0 a0, const _A1& a1) { return ::opencv_m_up::up_mul_d_e(a0, a1); }
template<class _A0, class _A1> requires (std::is_arithmetic_v<_A0> && __is_same(_A1, cv::MatExpr))
inline cv::MatExpr operator / (_A0 a0, const _A1& a1) { return ::opencv_m_up::up_div_d_e(a0, a1); }
} // inline namespace opencv_m_repl
} // export namespace cv
