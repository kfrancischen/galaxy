#include <string>
#include <gtest/gtest.h>
#include "cpp/internal/galaxy_fs_internal.h"

namespace {

    TEST(GalaxyFsInternalTest, JoinPath) {
        std::string output = galaxy::internal::JoinPath("/home", "test");
        EXPECT_EQ(output, "/home/test");
        output = galaxy::internal::JoinPath("/home/", "test");
        EXPECT_EQ(output, "/home/test");
    }

    TEST(GalaxyFsInternalTest, GetFileAbsDir) {
        auto output = galaxy::internal::GetFileAbsDir("/home/test");
        EXPECT_TRUE(output.ok());
        EXPECT_EQ(*output, "/home");

        output = galaxy::internal::GetFileAbsDir("home/test");
        EXPECT_TRUE(output.ok());
        EXPECT_EQ(*output, "home");

        output = galaxy::internal::GetFileAbsDir("");
        EXPECT_FALSE(output.ok());
    }

    TEST(GalaxyFsInternalTest, GetFileName) {
        auto output = galaxy::internal::GetFileName("/home/test");
        EXPECT_TRUE(output.ok());
        EXPECT_EQ(*output, "test");

        output = galaxy::internal::GetFileName("home/test");
        EXPECT_TRUE(output.ok());
        EXPECT_EQ(*output, "test");

        output = galaxy::internal::GetFileName("home/test/");
        EXPECT_FALSE(output.ok());

    }

    TEST(GalaxyFsInternalTest, GetFileLockName) {
        auto output = galaxy::internal::GetFileLockName("/home/test");
        EXPECT_TRUE(output.ok());
        EXPECT_EQ(*output, "/home/.test.lock");

        output = galaxy::internal::GetFileLockName("home/test");
        EXPECT_TRUE(output.ok());
        EXPECT_EQ(*output, "home/.test.lock");

        output = galaxy::internal::GetFileLockName("home/test/");
        EXPECT_FALSE(output.ok());
    }

}  // namespace