#include <string>
#include <gtest/gtest.h>
#include "cpp/core/galaxy_flag.h"
#include "cpp/util/galaxy_util.h"
#include "absl/flags/flag.h"

namespace {

    TEST(GalaxyUtilTest, GetAllCells) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        auto cells = galaxy::util::GetAllCells();
        EXPECT_EQ(cells[0], "zz");
        EXPECT_EQ(cells[1], "zzz");
    }

    TEST(GalaxyUtilTest, GetGalaxyFsPrefixPath) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        auto path = galaxy::util::GetGalaxyFsPrefixPath("zz");
        EXPECT_EQ(path, "/galaxy/zz-d");
    }

    TEST(GalaxyUtilTest, RunFileAnalyzerSuccess) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        auto result = galaxy::util::RunFileAnalyzer("/galaxy/zz-d/test");
        EXPECT_TRUE(result.ok());
        EXPECT_EQ(result->path(), "/home/galaxy/test");
    }

    TEST(GalaxyUtilTest, RunFileAnalyzerFailure) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        auto result = galaxy::util::RunFileAnalyzer("/galaxy/yz-d/test");
        EXPECT_FALSE(result.ok());
    }

    TEST(GalaxyUtilTest, ParseCellConfigSuccess) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        auto result = galaxy::util::ParseCellConfig("zz");
        EXPECT_TRUE(result.ok());
        EXPECT_EQ(result->fs_root(), "/home/galaxy");
    }

    TEST(GalaxyUtilTest, ParseCellConfigFailure) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        auto result = galaxy::util::ParseCellConfig("yz");
        EXPECT_FALSE(result.ok());
    }

    TEST(GalaxyUtilTest, InitClientCase1) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        std::string path = "/home/test";
        auto result = galaxy::util::InitClient(path);
        EXPECT_EQ(result.path(), "/home/test");
        EXPECT_FALSE(result.is_remote());
    }

    TEST(GalaxyUtilTest, InitClientCase2) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        std::string path = "/galaxy/zz-d/test";
        auto result = galaxy::util::InitClient(path);
        EXPECT_EQ(result.path(), "/home/galaxy/test");
        EXPECT_FALSE(result.is_remote());
    }

    TEST(GalaxyUtilTest, InitClientCase3) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        std::string path = "test";
        auto result = galaxy::util::InitClient(path);
        EXPECT_EQ(result.path(), "test");
        EXPECT_FALSE(result.is_remote());
    }

    TEST(GalaxyUtilTest, InitClientCase4) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        std::string path = "/galaxy/zzz-d/test";
        auto result = galaxy::util::InitClient(path);
        EXPECT_EQ(result.path(), "/home/galaxy/test");
        EXPECT_TRUE(result.is_remote());
    }

    TEST(GalaxyUtilTest, InitClientCase5) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        std::string path = "/SHARED/test";
        auto result = galaxy::util::InitClient(path);
        EXPECT_EQ(result.path(), "/home/galaxy/test");
        EXPECT_FALSE(result.is_remote());
        EXPECT_TRUE(result.is_shared());
    }

    TEST(GalaxyUtilTest, ConvertToCellPathCase1) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        std::string path = "/home/galaxy/test";
        auto config = galaxy::util::ParseCellConfig("zz");
        auto result = galaxy::util::ConvertToCellPath(path, *config);
        EXPECT_EQ(result, "/galaxy/zz-d/test");
    }

    TEST(GalaxyUtilTest, ConvertToCellPathCase2) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zz", 1);
        std::string path = "/root/test";
        auto config = galaxy::util::ParseCellConfig("zz");
        auto result = galaxy::util::ConvertToCellPath(path, *config);
        EXPECT_EQ(result, "/root/test");
    }

}  // namespace
