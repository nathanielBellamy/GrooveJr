#include <gtest/gtest.h>
#include "gui/Color.h"

using Gj::Gui::Color;
using Gj::Gui::GjC;

TEST(ColorTest, ToHex_AllColors) {
    EXPECT_EQ(Color::toHex(GjC::DARK_500), "#2D2D2D");
    EXPECT_EQ(Color::toHex(GjC::DARK_400), "#2A363B");
    EXPECT_EQ(Color::toHex(GjC::DARK_300), "#594F4F");
    EXPECT_EQ(Color::toHex(GjC::DARK_200), "#645757");
    EXPECT_EQ(Color::toHex(GjC::DARK_100), "#9DE0AD");
    EXPECT_EQ(Color::toHex(GjC::LIGHT_400), "#00948E");
    EXPECT_EQ(Color::toHex(GjC::LIGHT_300), "#45ADA8");
    EXPECT_EQ(Color::toHex(GjC::LIGHT_200), "#41C2AB");
    EXPECT_EQ(Color::toHex(GjC::LIGHT_100), "#E5EEC1");
    EXPECT_EQ(Color::toHex(GjC::MUTE_BLUE), "#55A0FE");
    EXPECT_EQ(Color::toHex(GjC::SOLO_YELLOW), "#F7DB69");
    EXPECT_EQ(Color::toHex(GjC::ENABLED_GREEN), "#2BF912");
}

TEST(ColorTest, HexValues_AreSevenChars) {
    for (const auto& [key, hex] : Color::hexs) {
        EXPECT_EQ(hex.size(), 7u) << "Hex value wrong length for color";
        EXPECT_EQ(hex[0], '#') << "Hex value missing # prefix";
    }
}

TEST(ColorTest, MapHas12Entries) {
    EXPECT_EQ(Color::hexs.size(), 12u);
}
