#include <gtest/gtest.h>
#include "audio/engine/Math.h"

using Gj::Audio::Math;

// ── floatToUInt127 ──

TEST(MathTest, FloatToUInt127_Zero) {
    EXPECT_EQ(Math::floatToUInt127(0.0f), 0);
}

TEST(MathTest, FloatToUInt127_One) {
    EXPECT_EQ(Math::floatToUInt127(1.0f), 100);
}

TEST(MathTest, FloatToUInt127_Half) {
    EXPECT_EQ(Math::floatToUInt127(0.5f), 50);
}

TEST(MathTest, FloatToUInt127_ClampsAbove127) {
    EXPECT_EQ(Math::floatToUInt127(2.0f), 127);
    EXPECT_EQ(Math::floatToUInt127(100.0f), 127);
}

TEST(MathTest, FloatToUInt127_ClampsNegativeToZero) {
    EXPECT_EQ(Math::floatToUInt127(-0.5f), 0);
    EXPECT_EQ(Math::floatToUInt127(-100.0f), 0);
}

TEST(MathTest, FloatToUInt127_BoundaryAt127) {
    EXPECT_EQ(Math::floatToUInt127(1.27f), 127);
}

// ── floatToInt127 ──

TEST(MathTest, FloatToInt127_Zero) {
    EXPECT_EQ(Math::floatToInt127(0.0f), 0);
}

TEST(MathTest, FloatToInt127_Positive) {
    EXPECT_EQ(Math::floatToInt127(1.0f), 100);
    EXPECT_EQ(Math::floatToInt127(0.5f), 50);
}

TEST(MathTest, FloatToInt127_Negative) {
    EXPECT_EQ(Math::floatToInt127(-1.0f), -100);
    EXPECT_EQ(Math::floatToInt127(-0.5f), -50);
}

TEST(MathTest, FloatToInt127_ClampsAbove127) {
    EXPECT_EQ(Math::floatToInt127(2.0f), 127);
}

TEST(MathTest, FloatToInt127_ClampsBelow) {
    EXPECT_EQ(Math::floatToInt127(-2.0f), -127);
}

// ── uInt127ToFloat ──

TEST(MathTest, UInt127ToFloat_Zero) {
    EXPECT_FLOAT_EQ(Math::uInt127ToFloat(0), 0.0f);
}

TEST(MathTest, UInt127ToFloat_Hundred) {
    EXPECT_FLOAT_EQ(Math::uInt127ToFloat(100), 1.0f);
}

TEST(MathTest, UInt127ToFloat_127) {
    EXPECT_FLOAT_EQ(Math::uInt127ToFloat(127), 1.27f);
}

TEST(MathTest, UInt127ToFloat_NegativeClampsToZero) {
    EXPECT_FLOAT_EQ(Math::uInt127ToFloat(-1), 0.0f);
    EXPECT_FLOAT_EQ(Math::uInt127ToFloat(-100), 0.0f);
}

TEST(MathTest, UInt127ToFloat_AboveClampsTo127) {
    EXPECT_FLOAT_EQ(Math::uInt127ToFloat(200), 1.27f);
}

// ── int127ToFloat ──

TEST(MathTest, Int127ToFloat_Zero) {
    EXPECT_FLOAT_EQ(Math::int127ToFloat(0), 0.0f);
}

TEST(MathTest, Int127ToFloat_Positive) {
    EXPECT_FLOAT_EQ(Math::int127ToFloat(100), 1.0f);
    EXPECT_FLOAT_EQ(Math::int127ToFloat(50), 0.5f);
}

TEST(MathTest, Int127ToFloat_Negative) {
    EXPECT_FLOAT_EQ(Math::int127ToFloat(-100), -1.0f);
    EXPECT_FLOAT_EQ(Math::int127ToFloat(-50), -0.5f);
}

TEST(MathTest, Int127ToFloat_ClampsAbove) {
    EXPECT_FLOAT_EQ(Math::int127ToFloat(200), 1.27f);
}

TEST(MathTest, Int127ToFloat_ClampsBelow) {
    EXPECT_FLOAT_EQ(Math::int127ToFloat(-200), -1.27f);
}

// ── Round-trip consistency (±1 tolerance due to float→floor precision) ──

TEST(MathTest, RoundTrip_UInt127) {
    for (int i = 0; i <= 127; ++i) {
        float f = Math::uInt127ToFloat(i);
        int back = Math::floatToUInt127(f);
        EXPECT_NEAR(back, i, 1) << "Round-trip exceeded ±1 for i=" << i;
    }
}

TEST(MathTest, RoundTrip_Int127_Positive) {
    for (int i = 0; i <= 127; ++i) {
        float f = Math::int127ToFloat(i);
        int back = Math::floatToInt127(f);
        EXPECT_NEAR(back, i, 1) << "Round-trip exceeded ±1 for i=" << i;
    }
}

TEST(MathTest, RoundTrip_Int127_Negative) {
    for (int i = -127; i < 0; ++i) {
        float f = Math::int127ToFloat(i);
        int back = Math::floatToInt127(f);
        EXPECT_NEAR(back, i, 1) << "Round-trip exceeded ±1 for i=" << i;
    }
}
