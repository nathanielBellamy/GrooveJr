#include <gtest/gtest.h>
#include "enums/PlayState.h"

using namespace Gj;

// ── psToInt ──

TEST(PlayStateTest, PsToInt_Stop) {
    EXPECT_EQ(psToInt(STOP), 0);
}

TEST(PlayStateTest, PsToInt_Play) {
    EXPECT_EQ(psToInt(PLAY), 1);
}

TEST(PlayStateTest, PsToInt_Pause) {
    EXPECT_EQ(psToInt(PAUSE), 2);
}

TEST(PlayStateTest, PsToInt_Rw) {
    EXPECT_EQ(psToInt(RW), 3);
}

TEST(PlayStateTest, PsToInt_Ff) {
    EXPECT_EQ(psToInt(FF), 4);
}

// ── intToPs ──

TEST(PlayStateTest, IntToPs_ValidValues) {
    EXPECT_EQ(intToPs(0), STOP);
    EXPECT_EQ(intToPs(1), PLAY);
    EXPECT_EQ(intToPs(2), PAUSE);
    EXPECT_EQ(intToPs(3), RW);
    EXPECT_EQ(intToPs(4), FF);
}

TEST(PlayStateTest, IntToPs_InvalidDefaultsToStop) {
    EXPECT_EQ(intToPs(-1), STOP);
    EXPECT_EQ(intToPs(5), STOP);
    EXPECT_EQ(intToPs(100), STOP);
    EXPECT_EQ(intToPs(-999), STOP);
}

// ── Round-trip ──

TEST(PlayStateTest, RoundTrip) {
    PlayState states[] = {STOP, PLAY, PAUSE, RW, FF};
    for (auto ps : states) {
        EXPECT_EQ(intToPs(psToInt(ps)), ps);
    }
}
