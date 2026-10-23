#include <gtest/gtest.h>
#include <array>
#include <cstdint>

import mcpplibs.mwrap.opencv;

namespace macros = mcpplibs::mwrap::opencv::macros;

TEST(MacroLayout, EveryOpenCv5DepthMatchesActualMatStorage) {
    constexpr std::array depths{macros::CV_8U, macros::CV_8S, macros::CV_16U,
        macros::CV_16S, macros::CV_32S, macros::CV_32F, macros::CV_64F,
        macros::CV_16F, macros::CV_16BF, macros::CV_Bool, macros::CV_64U,
        macros::CV_64S, macros::CV_32U};
    constexpr std::array sizes{1, 1, 2, 2, 4, 4, 8, 2, 2, 1, 8, 8, 4};
    static_assert(depths.size() == macros::CV_DEPTH_CURR_MAX);
    for (unsigned depth = 0; depth < depths.size(); ++depth) {
        EXPECT_EQ(depths[depth], depth);
        for (int channels : {1, 2, 3, 4, 7, macros::CV_CN_MAX}) {
            const auto type = macros::CV_MAKE_TYPE(depths[depth], channels);
            cv::Mat matrix(3, 5, type);
            EXPECT_EQ(matrix.depth(), depths[depth]);
            EXPECT_EQ(matrix.channels(), channels);
            EXPECT_EQ(matrix.elemSize1(), sizes[depth]);
            EXPECT_EQ(matrix.elemSize(), sizes[depth] * channels);
            EXPECT_EQ(macros::CV_MAT_DEPTH(matrix.flags), matrix.depth());
            EXPECT_EQ(macros::CV_MAT_CN(matrix.flags), matrix.channels());
            EXPECT_EQ(macros::CV_MAT_TYPE(matrix.flags), matrix.type());
            EXPECT_EQ(macros::CV_ELEM_SIZE1(type), matrix.elemSize1());
            EXPECT_EQ(macros::CV_ELEM_SIZE(type), matrix.elemSize());
        }
    }
}

TEST(MacroLayout, NewDepthFamiliesAndStorageAccess) {
    static_assert(macros::CV_16BFC(3) == macros::CV_16BFC3);
    static_assert(macros::CV_BoolC(4) == macros::CV_BoolC4);
    static_assert(macros::CV_64UC(2) == macros::CV_64UC2);
    static_assert(macros::CV_64SC(1) == macros::CV_64SC1);
    static_assert(macros::CV_32UC(3) == macros::CV_32UC3);
    static_assert(macros::CV_8UC(3) == macros::CV_8UC3);
    static_assert(macros::CV_IS_INT_TYPE(macros::CV_64UC1));
    static_assert(macros::CV_IS_FLOAT_TYPE(macros::CV_16BFC1));
    static_assert(!macros::CV_IS_FLOAT_TYPE(macros::CV_BoolC1));
    cv::Mat unsigned64(2, 3, macros::CV_64UC1);
    unsigned64.ptr<std::uint64_t>(1)[2] = UINT64_C(0xfedcba9876543210);
    EXPECT_EQ(unsigned64.ptr<std::uint64_t>(1)[2], UINT64_C(0xfedcba9876543210));
    cv::Mat signed64(2, 3, macros::CV_64SC1);
    signed64.ptr<std::int64_t>(1)[2] = -INT64_C(0x123456789abcdef);
    EXPECT_EQ(signed64.ptr<std::int64_t>(1)[2], -INT64_C(0x123456789abcdef));
    cv::Mat unsigned32(2, 3, macros::CV_32UC1);
    unsigned32.ptr<std::uint32_t>(1)[2] = UINT32_C(0xfedcba98);
    EXPECT_EQ(unsigned32.ptr<std::uint32_t>(1)[2], UINT32_C(0xfedcba98));
    cv::Mat boolean(2, 3, macros::CV_BoolC1);
    boolean.ptr<bool>(1)[2] = true;
    EXPECT_TRUE(boolean.ptr<bool>(1)[2]);
}

TEST(MacroLayout, RoiAndContinuityFlagsMatchMat) {
    cv::Mat parent(8, 10, macros::CV_8UC3);
    cv::Mat roi = parent(cv::Rect(2, 2, 4, 3));
    EXPECT_EQ(macros::CV_IS_SUBMAT(parent.flags) != 0, parent.isSubmatrix());
    EXPECT_EQ(macros::CV_IS_SUBMAT(roi.flags) != 0, roi.isSubmatrix());
    EXPECT_EQ(macros::CV_IS_MAT_CONT(parent.flags) != 0, parent.isContinuous());
    EXPECT_EQ(macros::CV_IS_CONT_MAT(roi.flags) != 0, roi.isContinuous());
    EXPECT_FALSE(roi.isContinuous());
    EXPECT_TRUE(roi.isSubmatrix());
}
