#include <gtest/gtest.h>
#include "types/AtomicStr.h"

using Gj::AtomicStr;
using Gj::ATOMIC_STR_SIZE;

// ── Constructors ──

TEST(AtomicStrTest, DefaultConstructor_FirstCharIsSpace) {
    AtomicStr s;
    EXPECT_EQ(s.value[0], ' ');
}

TEST(AtomicStrTest, CStrConstructor) {
    AtomicStr s("hello");
    EXPECT_STREQ(s.c_str(), "hello");
}

TEST(AtomicStrTest, StdStringConstructor) {
    std::string input = "world";
    AtomicStr s(input);
    std::string result = s.std_str();
    EXPECT_NE(result.find("world"), std::string::npos);
}

TEST(AtomicStrTest, EmptyString) {
    AtomicStr s("");
    // Default-initialized value[0] is ' '
    EXPECT_EQ(s.value[0], ' ');
}

// ── Accessors ──

TEST(AtomicStrTest, CStr_ReturnsPointerToValue) {
    AtomicStr s("test");
    EXPECT_EQ(s.c_str(), &s.value[0]);
}

TEST(AtomicStrTest, StdStr_ContainsContent) {
    AtomicStr s("foobar");
    std::string result = s.std_str();
    EXPECT_TRUE(result.find("foobar") == 0);
}

// ── Operators ──

TEST(AtomicStrTest, OperatorPlus_StringLeft) {
    AtomicStr s("world");
    std::string result = std::string("hello ") + s;
    EXPECT_NE(result.find("hello "), std::string::npos);
    EXPECT_NE(result.find("world"), std::string::npos);
}

TEST(AtomicStrTest, OperatorPlus_StringRight) {
    AtomicStr s("hello");
    std::string result = s + std::string(" world");
    EXPECT_NE(result.find("hello"), std::string::npos);
    EXPECT_NE(result.find(" world"), std::string::npos);
}

// ── Truncation ──

TEST(AtomicStrTest, Truncation_CStr) {
    std::string longStr(ATOMIC_STR_SIZE + 100, 'x');
    AtomicStr s(longStr.c_str());
    // Only first ATOMIC_STR_SIZE chars should be stored
    for (size_t i = 0; i < ATOMIC_STR_SIZE; ++i) {
        EXPECT_EQ(s.value[i], 'x') << "Mismatch at index " << i;
    }
}

TEST(AtomicStrTest, Truncation_StdString) {
    std::string longStr(ATOMIC_STR_SIZE + 100, 'y');
    AtomicStr s(longStr);
    for (size_t i = 0; i < ATOMIC_STR_SIZE; ++i) {
        EXPECT_EQ(s.value[i], 'y') << "Mismatch at index " << i;
    }
}

// ── Size constant ──

TEST(AtomicStrTest, SizeConstant) {
    EXPECT_EQ(ATOMIC_STR_SIZE, 512u);
}
