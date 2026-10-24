#pragma once
namespace simd_fixture {
inline void require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
template<class T, std::size_t N> void values(std::ostringstream& out, const std::array<T, N>& data) {
    for (const auto value : data) out << +value << ',';
    out << '\n';
}
inline void emit(const char* consumer) {
    static_assert(sizeof(cv::v_float32x4) == 16 && sizeof(cv::v_uint8x16) == 16);
    require(cv::VTraits<cv::v_float32>::vlanes() == 4, "baseline float lanes");
    require(cv::VTraits<cv::v_uint8>::vlanes() == 16, "baseline byte lanes");
    std::ostringstream out;
    out << std::setprecision(17);
    std::array<float, 5> input{99, 1.25f, -2.5f, 4.f, 9.f};
    std::array<float, 4> floats{};
    auto a = cv::v_load(input.data() + 1);
    auto b = cv::v_setall_f32(2.f);
    cv::v_store(floats.data(), cv::v_add(a, b));
    require(floats == std::array<float, 4>{3.25f, -0.5f, 6.f, 11.f}, "unaligned load/add/store");
    values(out, floats);
    std::array<float, 5> unaligned_output{};
    cv::v_store(unaligned_output.data() + 1, a);
    require(unaligned_output[1] == 1.25f && unaligned_output[4] == 9.f, "unaligned destination store");
    cv::v_store(floats.data(), cv::v_fma(a, b, cv::v_setall_f32(0.5f)));
    require(floats == std::array<float, 4>{3.f, -4.5f, 8.5f, 18.5f}, "multiply/add");
    values(out, floats);
    cv::v_store(floats.data(), cv::v_div(a, b));
    values(out, floats);
    std::array<uchar, 16> bytes{}, wrapped{};
    const auto high = cv::v_setall_u8(250), ten = cv::v_setall_u8(10);
    cv::v_store(bytes.data(), cv::v_add(high, ten));
    cv::v_store(wrapped.data(), cv::v_add_wrap(high, ten));
    require(bytes[0] == 255 && bytes[15] == 255 && wrapped[0] == 4, "byte saturation versus wrapping");
    values(out, bytes); values(out, wrapped);
    std::array<short, 8> signed_values{-5, 0, 5, 255, 256, 300, 1000, 42};
    const auto wide = cv::v_load(signed_values.data());
    cv::v_store(bytes.data(), cv::v_pack_u(wide, wide));
    require(bytes[0] == 0 && bytes[3] == 255 && bytes[4] == 255 && bytes[7] == 42, "unsigned saturating pack");
    values(out, bytes);
    cv::v_uint16x8 low, upper;
    cv::v_expand(cv::v_load(bytes.data()), low, upper);
    std::array<ushort, 8> expanded{};
    cv::v_store(expanded.data(), low);
    require(expanded[4] == 255 && expanded[7] == 42, "widen low lanes");
    values(out, expanded);
    cv::v_store(expanded.data(), upper); values(out, expanded);
    std::array<int, 4> left{1, -2, 3, 4}, right{1, 2, 0, 4}, integers{};
    auto x = cv::v_load(left.data()), y = cv::v_load(right.data());
    cv::v_store(integers.data(), cv::v_eq(x, y));
    require(integers == std::array<int, 4>{-1, 0, 0, -1}, "comparison lane masks");
    values(out, integers);
    cv::v_store(integers.data(), cv::v_select(cv::v_gt(x, y), x, y));
    require(integers == std::array<int, 4>{1, 2, 3, 4}, "mask selection");
    values(out, integers);
    cv::v_store(integers.data(), cv::v_reverse(x));
    require(integers == std::array<int, 4>{4, 3, -2, 1}, "lane reversal");
    values(out, integers);
    cv::v_store(integers.data(), cv::v_extract<1>(x, y));
    require(integers == std::array<int, 4>{-2, 3, 4, 1}, "lane extraction");
    values(out, integers);
    cv::v_int32x4 zipped_low, zipped_high;
    cv::v_zip(x, y, zipped_low, zipped_high);
    cv::v_store(integers.data(), zipped_low); values(out, integers);
    cv::v_store(integers.data(), zipped_high); values(out, integers);
    std::array<float, 4> halves{2.5f, -2.5f, 3.5f, -3.5f};
    cv::v_store(integers.data(), cv::v_round(cv::v_load(halves.data())));
    require(integers == std::array<int, 4>{2, -2, 4, -4}, "nearest-even rounding");
    values(out, integers);
    std::array<short, 8> sequence{1, 2, 3, 4, 5, 6, 7, 8};
    cv::v_store(integers.data(), cv::v_dotprod(cv::v_load(sequence.data()), cv::v_setall_s16(1)));
    require(integers == std::array<int, 4>{3, 7, 11, 15}, "adjacent dot products");
    values(out, integers);
    require(cv::v_reduce_sum(x) == 6, "integer reduction");
    std::array<int64, 2> signed64{-5, 7}, comparison64{};
    cv::v_store(comparison64.data(), cv::v_eq(cv::v_load(signed64.data()), cv::v_setall_s64(7)));
    require(comparison64[0] == 0 && comparison64[1] == -1, "64-bit comparisons");
    values(out, comparison64);
    std::array<double, 2> squares{4, 9}, doubles{};
    cv::v_store(doubles.data(), cv::v_sqrt(cv::v_load(squares.data())));
    require(doubles == std::array<double, 2>{2, 3}, "double precision square roots");
    values(out, doubles);
    std::array<uchar, 48> rgb{}, restored{};
    for (std::size_t i = 0; i < rgb.size(); ++i) rgb[i] = static_cast<uchar>(i);
    cv::v_uint8x16 red, green, blue;
    cv::v_load_deinterleave(rgb.data(), red, green, blue);
    cv::v_store_interleave(restored.data(), red, green, blue);
    require(rgb == restored, "three-channel interleaved roundtrip");
    values(out, restored);
    alignas(16) std::array<float, 4> aligned{};
    cv::vx_store_aligned(aligned.data(), cv::vx_load(input.data() + 1));
    require(aligned[0] == 1.25f && aligned[3] == 9.f, "legacy aligned store forward");
    cv::vx_store(floats.data(), cv::vx_load(input.data() + 1)); values(out, floats);
#if defined(MCPP_FEATURE_SIMD_EMULATED)
    // Keep the SDK's generic lane-count templates, beyond the 128-bit aliases.
    std::array<float, 8> lanes{1, 2, 3, 4, 5, 6, 7, 8};
    const cv::v_reg<float, 8> generic(lanes.data());
    auto twice = cv::v_add(generic, generic);
    require(twice.s[7] == 16, "generic eight-lane arithmetic");
    auto quotient = cv::v_div(twice, generic);
    auto difference = cv::v_sub(twice, generic);
    auto product = cv::v_mul(generic, quotient);
    auto low_double = cv::v_cvt_f64(twice);
    auto high_double = cv::v_cvt_f64_high(twice);
    require(low_double.s[3] == 8 && high_double.s[0] == 10, "generic double conversion halves");
    require(difference.s[6] == 7 && product.s[5] == 12, "generic arithmetic overloads");
    std::array<unsigned, 8> bit_lanes{1, 2, 3, 4, 5, 6, 7, 8};
    const cv::v_reg<unsigned, 8> bits(bit_lanes.data());
    auto signed_bits = cv::v_reinterpret_as_int(generic);
    auto mask_bits = cv::v_not(cv::v_xor(bits, bits));
    auto all_bits = cv::v_or(bits, mask_bits);
    auto unchanged_bits = cv::v_and(bits, mask_bits);
    require(mask_bits.s[0] == ~0u && all_bits.s[7] == ~0u && unchanged_bits.s[4] == bits.s[4], "generic bitwise templates");
    cv::v_reg<float, 8> minima, maxima;
    cv::v_minmax(generic, twice, minima, maxima);
    cv::v_reg<double, 4> horizontal;
    cv::v_hsum(generic, horizontal);
    std::array<int, 8> rounding{1, 2, 3, 4, 5, 6, 7, 8};
    auto shifted = cv::v_rshr<1>(cv::v_reg<int, 8>(rounding.data()));
    require(minima.s[7] == 8 && maxima.s[7] == 16 && horizontal.s[3] == 15 && shifted.s[6] == 4, "generic reductions and rounding shift");
    for (int i = 0; i < 8; ++i)
        out << twice.s[i] << ',' << quotient.s[i] << ',' << signed_bits.s[i] << ',' << shifted.s[i] << ';';
    out << '\n' << low_double.s[3] << ',' << high_double.s[0] << ',' << horizontal.s[3] << '\n';
#endif
    if (const char* directory = std::getenv("OPENCV_M_SIMD_OUTPUT")) {
        std::ofstream file(std::filesystem::path(directory) / (std::string(consumer) + ".txt"), std::ios::binary);
        file << out.str(); require(bool(file), "SIMD oracle output");
    }
}
}
