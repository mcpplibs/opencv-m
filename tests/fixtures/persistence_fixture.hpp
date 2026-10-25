#pragma once
// Included after either the native SDK header or the module import. The two
// independent consumers must produce byte-identical SDK serialization.
namespace persistence_fixture {
enum class Mode { high = 7 };
struct Value { int first, second; };
inline void write(cv::FileStorage& storage, const cv::String& name, const Value& value) {
    cv::write(storage, name, cv::Vec2i(value.first, value.second));
}
inline void read(const cv::FileNode& node, Value& value, const Value& fallback) {
    cv::Vec2i fields;
    cv::read(node, fields, cv::Vec2i(fallback.first, fallback.second));
    value = {fields[0], fields[1]};
}
inline std::string serialize(int format) {
    cv::FileStorage output("generated", cv::FileStorage::WRITE | cv::FileStorage::MEMORY | format);
    cv::write(output, "point", cv::Point2f(1.5f, -2.5f));
    cv::write(output, "point3", cv::Point3d(1, 2, 3));
    cv::write(output, "size", cv::Size2i(12, 8));
    cv::write(output, "rectangle", cv::Rect2d(1, 2, 3, 4));
    cv::write(output, "complex", cv::Complexd(3, -4));
    cv::write(output, "vector", cv::Vec3f(2, 4, 6));
    cv::write(output, "matrix", cv::Matx22d(1, 2, 3, 4));
    cv::write(output, "scalar", cv::Scalar(3, 5, 7, 9));
    cv::write(output, "range", cv::Range(2, 6));
    output << "integers" << std::vector<int>{3, 4, 5};
    output << "nested" << std::vector<std::vector<int>>{{1, 2}, {3}};
    output << "points" << std::vector<cv::Point2f>{cv::Point2f(1, 2), cv::Point2f(3, 4)};
    output << "matches" << std::vector<cv::DMatch>{cv::DMatch(0, 1, 2.5f)};
    output << "mode" << Mode::high;
    cv::write<Mode, nullptr>(output, "explicit_mode", Mode::high);
    output << "custom" << Value{17, 23};
    return output.releaseAndGetString();
}
inline void emit(const char* prefix) {
    const auto* directory = std::getenv("OPENCV_M_PERSISTENCE_OUTPUT");
    using Format = std::pair<const char*, int>;
    for (const auto [name, format] : {
            Format{"yaml", cv::FileStorage::FORMAT_YAML},
            Format{"xml", cv::FileStorage::FORMAT_XML},
            Format{"json", cv::FileStorage::FORMAT_JSON},
            Format{"yaml-base64", cv::FileStorage::FORMAT_YAML | cv::FileStorage::BASE64},
            Format{"xml-base64", cv::FileStorage::FORMAT_XML | cv::FileStorage::BASE64},
            Format{"json-base64", cv::FileStorage::FORMAT_JSON | cv::FileStorage::BASE64}}) {
        const auto text = serialize(format);
        if (text.empty()) throw std::runtime_error("empty serialization");
        cv::FileStorage input(text, cv::FileStorage::READ | cv::FileStorage::MEMORY);
        Value value{};
        input["custom"] >> value;
        if (value.first != 17 || value.second != 23) throw std::runtime_error("custom serializer mismatch");
        if (directory) {
            const auto file = std::filesystem::path(directory) / (std::string(prefix) + "-" + name + ".txt");
            std::ofstream output(file, std::ios::binary);
            output.write(text.data(), static_cast<std::streamsize>(text.size()));
            if (!output) throw std::runtime_error("cannot write serialization evidence");
        }
    }
}
}
