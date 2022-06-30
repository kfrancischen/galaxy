#include <string>
#include <gtest/gtest.h>
#include "cpp/core/galaxy_flag.h"
#include "cpp/util/galaxy_util.h"
#include "absl/flags/flag.h"

namespace {

    TEST(GalaxyUtilTest, GetCellAndPathFromPathRemote) {
        std::string path = "/galaxy/aa-d/test";
        auto result = galaxy::util::GetCellAndPathFromPath(path);
        EXPECT_TRUE(result.ok());
        EXPECT_EQ((*result).first, "aa");
        EXPECT_EQ((*result).second, "test");
    }

    TEST(GalaxyUtilTest, GetCellAndPathFromPathLocal) {
        std::string path = "/home/test";
        auto result = galaxy::util::GetCellAndPathFromPath(path);
        EXPECT_FALSE(result.ok());
    }

    TEST(GalaxyUtilTest, ConvertToCellPathInGalaxy) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        absl::SetFlag(&FLAGS_fs_cell, "zz");
        std::string path = "/home/galaxy/test";
        auto result = galaxy::util::ConvertToCellPath(path);
        EXPECT_EQ(result, "/galaxy/zz-d/test");
    }

    TEST(GalaxyUtilTest, ConvertToCellPathOutsideGalaxy) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        setenv("GALAXY_fs_cell", "zzz", 1);
        std::string path = "/mnt/galaxy/test";
        auto result = galaxy::util::ConvertToCellPath(path);
        EXPECT_EQ(result, "/mnt/galaxy/test");
    }

    TEST(GalaxyUtilTest, ConvertToLocalPathInGalaxy) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        absl::SetFlag(&FLAGS_fs_cell, "zz");
        setenv("GALAXY_fs_cell", "zz", 1);
        std::string path = "/galaxy/zz-d/test";
        auto result = galaxy::util::ConvertToLocalPath(path);
        EXPECT_TRUE(result.ok());
        EXPECT_EQ(*result, "/home/galaxy/test");
    }

    TEST(GalaxyUtilTest, ConvertToLocalPathLocal) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        absl::SetFlag(&FLAGS_fs_cell, "zz");
        setenv("GALAXY_fs_cell", "zz", 1);
        std::string path = "/LOCAL/test";
        auto result = galaxy::util::ConvertToLocalPath(path);
        EXPECT_TRUE(result.ok());
        EXPECT_EQ(*result, "/home/galaxy/test");
    }

    TEST(GalaxyUtilTest, ConvertToLocalPathDiffCell) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        absl::SetFlag(&FLAGS_fs_cell, "zz");
        setenv("GALAXY_fs_cell", "zz", 1);
        std::string path = "/galaxy/zzz-d/test";
        auto result = galaxy::util::ConvertToLocalPath(path);
        EXPECT_TRUE(result.ok());
        EXPECT_EQ(*result, "/galaxy/zzz-d/test");
    }

    TEST(GalaxyUtilTest, ConvertToLocalPathOutsideGalaxy) {
        absl::SetFlag(&FLAGS_fs_global_config, "cpp/util/test/config.json");
        absl::SetFlag(&FLAGS_fs_cell, "zz");
        setenv("GALAXY_fs_cell", "zz", 1);
        std::string path = "/home/test";
        auto result = galaxy::util::ConvertToLocalPath(path);
        EXPECT_TRUE(result.ok());
        EXPECT_EQ(*result, "/home/test");
    }

}  // namespace