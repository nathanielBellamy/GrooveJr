#include <gtest/gtest.h>
#include "Paths.h"

using Gj::Paths;

TEST(PathsTest, DataDir_NotEmpty) {
    auto dir = Paths::dataDir();
    EXPECT_FALSE(dir.empty());
}

TEST(PathsTest, DataDir_EndsWithGrooveJr) {
    auto dir = Paths::dataDir();
    EXPECT_EQ(dir.filename(), "GrooveJr");
}

TEST(PathsTest, DataDir_Exists) {
    auto dir = Paths::dataDir();
    EXPECT_TRUE(std::filesystem::exists(dir));
}

TEST(PathsTest, LogDir_NotEmpty) {
    auto dir = Paths::logDir();
    EXPECT_FALSE(dir.empty());
}

TEST(PathsTest, LogDir_EndsWithGrooveJr) {
    auto dir = Paths::logDir();
    EXPECT_EQ(dir.filename(), "GrooveJr");
}

TEST(PathsTest, LogDir_Exists) {
    auto dir = Paths::logDir();
    EXPECT_TRUE(std::filesystem::exists(dir));
}

TEST(PathsTest, DatabasePath_EndsWithDb) {
    auto path = Paths::databasePath();
    EXPECT_EQ(path.filename(), "groovejr.db");
}

TEST(PathsTest, DatabasePath_ParentIsDataDir) {
    auto dbPath = Paths::databasePath();
    auto dataDir = Paths::dataDir();
    EXPECT_EQ(dbPath.parent_path(), dataDir);
}
