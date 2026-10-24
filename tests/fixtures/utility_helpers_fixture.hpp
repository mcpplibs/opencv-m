#pragma once

namespace utility_helpers_fixture {
inline void require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
struct Record { int id; float value; };

inline void emit(const char* consumer) {
    std::ostringstream result;
    result << std::setprecision(17);
    const short first[]{-3, 4, 0}, second[]{1, -2, 0};
    const auto l2 = cv::normL2Sqr<short, long long>(first, 3);
    const auto l1 = cv::normL1<short, long long>(first, 3);
    const auto inf = cv::normInf<short, long long>(first, 3);
    require(l2 == 25 && l1 == 7 && inf == 4, "single-array norms");
    const auto diff2 = cv::normL2Sqr<short, long long>(first, second, 3);
    const auto diff1 = cv::normL1<short, long long>(first, second, 3);
    const auto diff_inf = cv::normInf<short, long long>(first, second, 3);
    require(diff2 == 52 && diff1 == 10 && diff_inf == 6, "two-array norms");
    result << l2 << ',' << l1 << ',' << inf << ',' << diff2 << ',' << diff1 << ',' << diff_inf << '\n';
    const uint64 low[]{0, uint64(1) << 63}, high[]{std::numeric_limits<uint64>::max(), uint64(1) << 63};
    require(cv::normL1<uint64, uint64>(low, high, 2) == high[0], "unsigned norm must not overflow signed subtraction");
    require(cv::normInf<uint64, uint64>(low, high, 2) == high[0], "unsigned maximum distance");
    const float values[]{0.25f, -0.5f, 1.75f}, other[]{0.5f, 0.25f, -1.f};
    result << cv::normL2Sqr<float, double>(values, 3) << ','
           << cv::normL1<float, double>(values, 3) << ',' << cv::normInf<float, double>(values, 3) << ','
           << cv::normL2Sqr<float, double>(values, other, 3) << ','
           << cv::normL1<float, double>(values, other, 3) << ',' << cv::normInf<float, double>(values, other, 3) << '\n';
    require(cv::normL2Sqr<float, double>(nullptr, 0) == 0 &&
            cv::normL1<float, double>(nullptr, nullptr, 0) == 0 &&
            cv::normInf<float, double>(nullptr, 0) == 0, "empty norms");

    alignas(64) std::array<uchar, 128> storage{};
    const void* aligned = storage.data();
    const void* misaligned = storage.data() + 1;
    const auto address = reinterpret_cast<std::uintptr_t>(aligned);
    require((cv::isAligned<64, std::uintptr_t>(address)), "numeric alignment overload");
    require(cv::isAligned<64>(aligned) && cv::isAligned<64>(aligned, aligned) &&
            cv::isAligned<64>(aligned, aligned, aligned) &&
            cv::isAligned<64>(aligned, aligned, aligned, aligned), "pointer alignment overloads");
    require(!cv::isAligned<64>(misaligned) && !cv::isAligned<64>(aligned, misaligned) &&
            !cv::isAligned<64>(aligned, aligned, misaligned) &&
            !cv::isAligned<64>(aligned, aligned, aligned, misaligned), "misaligned pointers");
    result << "alignment: five valid/four rejected\n";
    const double distance = cv::jaccardDistance(cv::Rect(0, 0, 4, 4), cv::Rect(2, 2, 4, 4));
    require(std::abs(distance - 6.0 / 7.0) < 1e-14, "rectangle intersection/union distance");
    require(cv::jaccardDistance(cv::Rect(), cv::Rect()) == 0 &&
            cv::jaccardDistance(cv::Rect(0, 0, 1, 1), cv::Rect(2, 2, 1, 1)) == 1, "empty/disjoint rectangles");
    result << "jaccard: " << distance << '\n';
    const int largest = std::numeric_limits<int>::max();
    const double boundary = cv::jaccardDistance(cv::Rect(largest - 4, 0, 8, 1), cv::Rect(largest - 2, 0, 1, 1));
    require(boundary == 0.875, "SDK overflow-safe rectangle intersection");
    result << "jaccard boundary: " << boundary << '\n';
    cv::Point3i point(2, 3, 4);
    point *= 3;
    point -= cv::Point3i(1, 2, 3);
    point /= 2.0;
    require(point == cv::Point3i(2, 4, 4), "point compound overloads and nearest-even conversion");
    cv::Size size(3, 4);
    size += cv::Size(1, 2);
    size *= 2;
    size /= 2;
    require(size == cv::Size(4, 6), "size compound overloads");
    cv::Rect rectangle(1, 2, 3, 4);
    rectangle += cv::Point(2, 3);
    rectangle -= cv::Point(1, 1);
    rectangle += cv::Size(2, 2);
    rectangle -= cv::Size(1, 1);
    rectangle &= cv::Rect(3, 5, 8, 8);
    require(rectangle == cv::Rect(3, 5, 3, 4), "rectangle compound intersection");
    rectangle |= cv::Rect(0, 0, 1, 1);
    require(rectangle == cv::Rect(0, 0, 6, 9), "rectangle compound union");
    const cv::Scalar left(1, 2, 3, 4), right(5, 6, 7, 8);
    cv::Scalar product = left * right;
    require(product == cv::Scalar(-60, 12, 30, 24), "SDK scalar hypercomplex product");
    product /= right;
    for (int index = 0; index < 4; ++index)
        require(std::abs(product[index] - left[index]) < 1e-13, "SDK scalar quotient");
    cv::Complexd complex(2, 3), divisor(4, -1);
    complex = complex * divisor;
    require(complex == cv::Complexd(11, 10), "complex product");
    complex /= divisor;
    require(std::abs(complex.re - 2) < 1e-13 && std::abs(complex.im - 3) < 1e-13, "complex quotient");
    cv::Range range(1, 6);
    range &= cv::Range(3, 8);
    require(range == cv::Range(3, 6) && (2 + range) - 2 == range && !cv::Range(4, 4), "range overloads");
    result << "operators: " << point.x << ',' << point.y << ',' << point.z << ',' << size.width << ','
           << size.height << ',' << rectangle.width << ',' << rectangle.height << ',' << complex.re << ','
           << complex.im << ',' << range.start << ',' << range.end << '\n';

    const std::vector<Record> records{{3, 1.5f}, {7, -2.f}};
    const auto input = cv::rawIn(records).getMat();
    require(input.total() == records.size() && input.elemSize() == sizeof(Record) &&
            input.data == reinterpret_cast<const uchar*>(records.data()), "raw vector input identity");
    std::vector<Record> output;
    auto destination = cv::rawOut(output);
    destination.create(1, 2, input.type());
    auto view = destination.getMat();
    reinterpret_cast<Record*>(view.data)[0] = {9, 2.5f};
    reinterpret_cast<Record*>(view.data)[1] = {4, -1.f};
    auto writable = cv::rawInOut(output).getMat();
    reinterpret_cast<Record*>(writable.data)[1].id = 11;
    require(output.size() == 2 && output[0].id == 9 && output[0].value == 2.5f &&
            output[1].id == 11 && output[1].value == -1.f, "raw vector output identity");
    std::array<int, 2> array{13, 17};
    const auto array_input = cv::rawIn(array).getMat();
    require(array_input.type() == cv::traits::Type<int>::value && array_input.total() == 2,
            "raw std::array retains SDK element typing");
    auto array_output = cv::rawOut(array).getMat();
    reinterpret_cast<int*>(array_output.data)[0] = 19;
#if defined(OPENCV_M_NATIVE_REFERENCE)
    // The pinned header's free wrapper incorrectly calls this non-static
    // member without an instance. Compare the adapter with the SDK member.
    cv::_InputOutputArray owner;
    auto array_rw = owner.rawInOut(array).getMat();
#else
    auto array_rw = cv::rawInOut(array).getMat();
#endif
    reinterpret_cast<int*>(array_rw.data)[1] = 23;
    require(array[0] == 19 && array[1] == 23, "raw array storage identity");
    result << "raw: " << input.type() << ',' << input.total() << ',' << array_input.type() << ','
           << output[0].id << ',' << output[1].id << ',' << array[0] << ',' << array[1] << '\n';
    require(cvTrunc(cv::softfloat(2.9f)) == 2 && cvTrunc(cv::softdouble(-2.9)) == -2, "soft scalar truncation");
    require(cvRound64(cv::softdouble(2.5)) == 2 && cvRound64(cv::softdouble(-2.5)) == -2 &&
            cvRound64(cv::softdouble(4294967297.0)) == 4294967297LL, "soft scalar nearest-even int64");
    require(cv::utils::getThreadID() >= 0 && cv::utils::getThreadID() == cv::utils::getThreadID(), "thread identity");
    result << "scalar: 2,-2,2,-2,4294967297\n";
    for (double value : {-3.75, -2.5, -0.0, 0.0, 2.5, 3.75}) {
        const float narrow = static_cast<float>(value);
        const int rounded = ::cvRound(value), floor = ::cvFloor(value), ceil = ::cvCeil(value);
        require(rounded == ::cvRound(narrow) && floor == ::cvFloor(narrow) && ceil == ::cvCeil(narrow),
                "global fast math float/double overloads");
        require(rounded == ::cvRound(cv::softdouble(value)) && rounded == ::cvRound(cv::softfloat(narrow)) &&
                floor == ::cvFloor(cv::softdouble(value)) && floor == ::cvFloor(cv::softfloat(narrow)) &&
                ceil == ::cvCeil(cv::softdouble(value)) && ceil == ::cvCeil(cv::softfloat(narrow)),
                "global fast math SoftFloat overloads");
        result << "round: " << value << ',' << rounded << ',' << floor << ',' << ceil << '\n';
    }
    for (int value : {std::numeric_limits<int>::min(), -1, 0, 1, std::numeric_limits<int>::max()})
        require(::cvRound(value) == value && ::cvFloor(value) == value && ::cvCeil(value) == value,
                "integer fast math identity");
    require(::cvIsInf(std::numeric_limits<double>::infinity()) &&
            ::cvIsInf(-std::numeric_limits<float>::infinity()) &&
            ::cvIsNaN(std::numeric_limits<double>::quiet_NaN()) &&
            ::cvIsNaN(std::numeric_limits<float>::quiet_NaN()) &&
            !::cvIsInf(0.0) && !::cvIsInf(-0.f) && !::cvIsNaN(0.0) && !::cvIsNaN(-0.f),
            "global infinity/NaN classification");
    auto round_address = static_cast<int (*)(double)>(&::cvRound);
    require(round_address(2.5) == 2, "fast math overload address");
    require(::CV_FOURCC('M', 'J', 'P', 'G') == 0x47504a4d &&
            ::CV_FOURCC(char(0xff), 'A', 'B', char(0x7f)) == 0x7f4241ff,
            "legacy global fourcc byte packing");
    result << "fast math: classification,integer,address,fourcc\n";
    OpenCV_API_Header header{sizeof(OpenCV_API_Header), 1, 2, 5, 0, 0, "", "mwrap fixture"};
    OpenCV_API_Header_t* same_header = &header;
    cvResult code = CV_ERROR_OK;
    CvResult same_code = code;
    require(same_header->valid_size == sizeof(OpenCV_API_Header_t) &&
            same_header->min_api_version == 1 && same_header->api_version == 2 &&
            same_header->opencv_version_major == 5 && same_code == 0 && CV_ERROR_FAIL == -1 &&
            std::string(same_header->api_description) == "mwrap fixture", "original preview plugin ABI types");
    result << "plugin ABI: " << sizeof(header) << ',' << sizeof(code) << ',' << CV_ERROR_FAIL << '\n';
    const int float_type = cv::traits::Type<float>::value;
    cv::Mat parent(4, 6, float_type), contiguous = parent(cv::Rect(1, 1, 4, 3));
    require(!contiguous.isContinuous(), "CPU input has row gaps");
    cv::cuda::createContinuous(cv::Size(4, 3), float_type, contiguous);
    require(contiguous.isContinuous() && contiguous.rows == 3 && contiguous.cols == 4 &&
            contiguous.type() == float_type, "CUDA namespace helper supports CPU Mat");
    auto* original_storage = contiguous.data;
    cv::cuda::ensureSizeIsEnough(cv::Size(3, 2), float_type, contiguous);
    require(contiguous.data == original_storage && contiguous.rows == 2 && contiguous.cols == 3,
            "CPU buffer reuse through Size overload");
    cv::cuda::ensureSizeIsEnough(5, 7, float_type, contiguous);
    require(contiguous.rows == 5 && contiguous.cols == 7 && contiguous.type() == float_type,
            "CPU buffer grows through dimensions overload");
    cv::cuda::createContinuous(7, 5, float_type, contiguous);
    require(contiguous.isContinuous() && contiguous.rows == 7 && contiguous.cols == 5,
            "CPU continuous buffer reshape");
    auto return_by_size = static_cast<cv::cuda::GpuMat (*)(cv::Size, int)>(&cv::cuda::createContinuous);
    auto return_by_dims = static_cast<cv::cuda::GpuMat (*)(int, int, int)>(&cv::cuda::createContinuous);
    require(return_by_size && return_by_dims, "GPU-returning overload signatures available");
    cv::cuda::GpuMat empty_first(nullptr), empty_second(nullptr);
    cv::cuda::swap(empty_first, empty_second);
    require(empty_first.empty() && empty_second.empty(), "empty GPU headers swap without device allocation");
    cv::cuda::HostMem locked(cv::cuda::HostMem::PAGE_LOCKED), shared(cv::cuda::HostMem::SHARED);
    cv::cuda::swap(locked, shared);
    require(locked.alloc_type == cv::cuda::HostMem::SHARED &&
            shared.alloc_type == cv::cuda::HostMem::PAGE_LOCKED, "host header allocation modes swap");
    using Event = cv::cuda::Event;
    Event::CreateFlags flags = Event::DEFAULT;
    require(!flags, "zero event flags");
    flags = Event::BLOCKING_SYNC | Event::DISABLE_TIMING;
    require(flags == 3 && flags != 0 && !(!flags), "event flags OR and logical comparisons");
    require((flags & Event::DISABLE_TIMING) == 2 && (flags ^ Event::DISABLE_TIMING) == 1 &&
            (~Event::DEFAULT) != 0, "event flags AND/XOR/NOT");
    flags &= Event::DISABLE_TIMING;
    flags |= Event::INTERPROCESS;
    flags ^= Event::DISABLE_TIMING;
    require(flags == 4, "event flag compound operators preserve enum type");
    result << "cuda CPU helpers: " << contiguous.rows << ',' << contiguous.cols << ',' << static_cast<int>(flags) << '\n';
    const std::vector<cv::String> descriptions{
        cv::utils::dumpBool(true), cv::utils::dumpBool(false), cv::utils::dumpInt(-17),
        cv::utils::dumpInt64(4294967297LL), cv::utils::dumpSizeT(4096),
        cv::utils::dumpFloat(2.375f), cv::utils::dumpDouble(-2.375),
        cv::utils::dumpCString("SDK"), cv::utils::dumpString("module"),
        cv::utils::dumpRect(cv::Rect(1, -2, 3, 4)),
        cv::utils::dumpTermCriteria(cv::TermCriteria(3, 10, 0.125)),
        cv::utils::dumpRotatedRect(cv::RotatedRect(cv::Point2f(1, 2), cv::Size2f(3, 4), 15)),
        cv::utils::dumpRange(cv::Range(2, 7)), cv::utils::dumpRange(cv::Range::all()),
        cv::utils::dumpVectorOfInt({-1, 2, 3}),
        cv::utils::dumpVectorOfDouble({0.125, -2.375}),
        cv::utils::dumpVectorOfRect({cv::Rect(1, 2, 3, 4)})};
    require(descriptions[0] == "Bool: True" && descriptions[1] == "Bool: False" &&
            descriptions[2] == "Int: -17" && descriptions[3] == "Int64: 4294967297" &&
            descriptions[4] == "size_t: 4096" && descriptions[12] == "range: (s=2, e=7)" &&
            descriptions[13] == "range: all", "public utility formatting contracts");
    for (const auto& text : descriptions) result << text << '\n';
    cv::Mat diagnostic(2, 3, cv::traits::Type<float>::value, cv::Scalar(7));
    std::vector<cv::Mat> diagnostic_arrays{diagnostic, diagnostic};
    const std::vector<cv::String> arrays{
        cv::utils::dumpInputArray(diagnostic), cv::utils::dumpInputArrayOfArrays(diagnostic_arrays),
        cv::utils::dumpInputOutputArray(diagnostic),
        cv::utils::dumpInputOutputArrayOfArrays(diagnostic_arrays)};
    for (const auto& text : arrays) {
        require(text.find("empty()=false") != cv::String::npos && text.find("ERROR") == cv::String::npos,
                "public array diagnostic helper");
        result << text << '\n';
    }
    auto cache_accessor = &cv::utils::fs::getCacheDirectoryForDownloads;
    require(cache_accessor != nullptr, "download cache accessor signature without filesystem side effects");
    if (const char* directory = std::getenv("OPENCV_M_UTILITY_OUTPUT")) {
        std::ofstream file(std::filesystem::path(directory) / (std::string(consumer) + ".txt"), std::ios::binary);
        file << result.str();
        require(bool(file), "utility oracle output");
    }
}
}
