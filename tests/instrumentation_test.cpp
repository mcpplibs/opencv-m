#include <gtest/gtest.h>
#include <string>

import mcpplibs.mwrap.opencv.core;

TEST(Instrumentation, PublicNodeRetainsSdkConstructionAndTiming) {
    cv::instr::NodeData node("module-consumer", "consumer.cpp", 42, nullptr,
                             false, cv::instr::TYPE_MARKER, cv::instr::IMPL_PLAIN);
    EXPECT_EQ(node.m_funName, "module-consumer");
    EXPECT_EQ(node.m_lineNum, 42);
    EXPECT_EQ(node.m_instrType, cv::instr::TYPE_MARKER);
    EXPECT_EQ(node.m_implType, cv::instr::IMPL_PLAIN);
    node.m_ticksTotal = static_cast<uint64>(cv::getTickFrequency());
    node.m_counter = 2;
    EXPECT_NEAR(node.getTotalMs(), 1000, 0.001);
    EXPECT_NEAR(node.getMeanMs(), 500, 0.001);
    cv::instr::NodeData copy(node);
    EXPECT_EQ(copy.m_funName, node.m_funName);
    EXPECT_EQ(copy.m_counter, 2);
    EXPECT_TRUE(node == copy);
    cv::instr::NodeDataTls local;
    EXPECT_EQ(local.m_ticksTotal, 0u);
}

TEST(Instrumentation, DisabledSdkContractIsPreservedRatherThanEmulated) {
    // The current SDK configuration does not enable ENABLE_INSTRUMENTATION.
    // These are upstream no-ops, not wrapper-owned state or tracing support.
    EXPECT_FALSE(cv::instr::useInstrumentation());
    cv::instr::setUseInstrumentation(true);
    EXPECT_FALSE(cv::instr::useInstrumentation());
    cv::instr::setFlags(cv::instr::FLAGS_MAPPING);
    EXPECT_EQ(cv::instr::getFlags(), cv::instr::FLAGS_NONE);
    cv::instr::setFlags(static_cast<int>(cv::instr::FLAGS_EXPAND_SAME_NAMES));
    EXPECT_EQ(cv::instr::getFlags(), cv::instr::FLAGS_NONE);
    EXPECT_EQ(cv::instr::getTrace(), nullptr);
    EXPECT_NO_THROW(cv::instr::resetTrace());
    cv::instr::setUseInstrumentation(false);
}
