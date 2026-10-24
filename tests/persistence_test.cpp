#include <gtest/gtest.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

import mcpplibs.mwrap.opencv.core;
#include "fixtures/persistence_fixture.hpp"

TEST(Persistence, XmlYamlJsonAndCustomSerializers) {
    EXPECT_NO_THROW(persistence_fixture::emit("module"));
}

namespace {
enum class Mode { low = 3, high = 7 };
}

namespace custom_record {
struct Value { int first, second; };
void write(cv::FileStorage& storage, const cv::String& name, const Value& value) {
    cv::write(storage, name, cv::Vec2i(value.first, value.second));
}
void read(const cv::FileNode& node, Value& value, const Value& fallback) {
    cv::Vec2i fields;
    cv::read(node, fields, cv::Vec2i(fallback.first, fallback.second));
    value = {fields[0], fields[1]};
}
}

TEST(Persistence, GeometryMatricesVectorsAndStreamsUseTheSdkFormat) {
    cv::FileStorage writer("generated.yml", cv::FileStorage::WRITE | cv::FileStorage::MEMORY);
    const cv::Point2f point(1.5f, -2.5f);
    const cv::Point3d point3(1, 2, 3);
    const cv::Size2i size(12, 8);
    const cv::Rect2d rectangle(1, 2, 3, 4);
    const cv::Complexd complex(3, -4);
    const cv::Vec3f vector(2, 4, 6);
    const cv::Matx22d matrix(1, 2, 3, 4);
    const cv::Scalar scalar(3, 5, 7, 9);
    cv::write(writer, "point", point);
    cv::write(writer, "point3", point3);
    cv::write(writer, "size", size);
    cv::write(writer, "rectangle", rectangle);
    cv::write(writer, "complex", complex);
    cv::write(writer, "vector", vector);
    cv::write(writer, "matrix", matrix);
    cv::write(writer, "scalar", scalar);
    cv::write(writer, "range", cv::Range(2, 6));
    writer << "integers" << std::vector<int>{3, 4, 5};
    writer << "points" << std::vector<cv::Point2f>{point, cv::Point2f(7, 8)};
    writer << "nested" << std::vector<std::vector<int>>{{1, 2}, {3}};
    writer << "matches" << std::vector<cv::DMatch>{cv::DMatch(0, 1, 2.5f)};
    writer << "mode" << Mode::high;
    writer << "custom" << custom_record::Value{17, 23};
    writer << "large" << 300;
    writer << "fallback" << "[" << 1 << "]";
    const auto text = writer.releaseAndGetString();
    ASSERT_NE(text.find("%YAML"), std::string::npos);
    cv::FileStorage reader(text, cv::FileStorage::READ | cv::FileStorage::MEMORY);
    cv::Point2f restoredPoint;
    cv::Point3d restoredPoint3;
    cv::Size2i restoredSize;
    cv::Rect2d restoredRect;
    cv::Complexd restoredComplex;
    cv::Vec3f restoredVector;
    cv::Matx22d restoredMatrix;
    cv::Scalar restoredScalar;
    cv::Range restoredRange;
    cv::read(reader["point"], restoredPoint, cv::Point2f());
    cv::read(reader["point3"], restoredPoint3, cv::Point3d());
    cv::read(reader["size"], restoredSize, cv::Size2i());
    cv::read(reader["rectangle"], restoredRect, cv::Rect2d());
    cv::read(reader["complex"], restoredComplex, cv::Complexd());
    cv::read(reader["vector"], restoredVector, cv::Vec3f());
    cv::read(reader["matrix"], restoredMatrix);
    cv::read(reader["scalar"], restoredScalar, cv::Scalar());
    cv::read(reader["range"], restoredRange, cv::Range());
    EXPECT_EQ(restoredPoint, point);
    EXPECT_EQ(restoredPoint3, point3);
    EXPECT_EQ(restoredSize, size);
    EXPECT_EQ(restoredRect, rectangle);
    EXPECT_DOUBLE_EQ(restoredComplex.re, complex.re);
    EXPECT_DOUBLE_EQ(restoredComplex.im, complex.im);
    EXPECT_EQ(cv::norm(restoredVector, vector), 0);
    EXPECT_EQ(cv::norm(restoredMatrix - matrix), 0);
    EXPECT_EQ(restoredScalar, scalar);
    EXPECT_EQ(restoredRange, cv::Range(2, 6));
    std::vector<int> integers;
    std::vector<cv::Point2f> points;
    reader["integers"] >> integers;
    reader["points"] >> points;
    EXPECT_EQ(integers, (std::vector<int>{3, 4, 5}));
    ASSERT_EQ(points.size(), 2u);
    EXPECT_EQ(points[0], point);
    EXPECT_EQ(points[1], cv::Point2f(7, 8));
    std::vector<std::vector<int>> nested;
    reader["nested"] >> nested;
    EXPECT_EQ(nested, (std::vector<std::vector<int>>{{1, 2}, {3}}));
    std::vector<cv::DMatch> matches;
    reader["matches"] >> matches;
    ASSERT_EQ(matches.size(), 1u);
    EXPECT_EQ(matches[0].queryIdx, 0);
    EXPECT_EQ(matches[0].trainIdx, 1);
    EXPECT_FLOAT_EQ(matches[0].distance, 2.5f);
    Mode mode = Mode::low;
    cv::read<Mode, nullptr>(reader["mode"], mode);
    EXPECT_EQ(mode, Mode::high);
    custom_record::Value custom{};
    reader["custom"] >> custom;
    EXPECT_EQ(custom.first, 17);
    EXPECT_EQ(custom.second, 23);
    uchar saturated = 0;
    cv::read(reader["large"], saturated, static_cast<uchar>(0));
    EXPECT_EQ(saturated, 255);
    cv::read(reader["missing"], restoredPoint, cv::Point2f(11, 13));
    EXPECT_EQ(restoredPoint, cv::Point2f(11, 13));
    cv::read(reader["fallback"], restoredPoint, point);
    EXPECT_EQ(restoredPoint, point);
    auto iterator = reader["integers"].begin();
    std::vector<int> prefix;
    cv::read(iterator, prefix, 2);
    EXPECT_EQ(prefix, (std::vector<int>{3, 4}));
    int last = 0;
    iterator >> last;
    EXPECT_EQ(last, 5);
    EXPECT_TRUE(iterator == reader["integers"].end());
    EXPECT_FALSE(iterator != reader["integers"].end());
    const auto begin = reader["integers"].begin();
    const auto end = reader["integers"].end();
    EXPECT_EQ(end - begin, 3);
    EXPECT_TRUE(begin < end);
}
