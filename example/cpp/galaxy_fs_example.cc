// To run this file:

// GLOG_logtostderr=1 bazel run -c opt //example/cpp:galaxy_fs_example --fs_root=/home/pslx/Download --mkdir_test=test/test


#include <iostream>
#include <string>
#include <vector>

#include "glog/logging.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "cpp/core/galaxy_fs.h"


ABSL_FLAG(std::string, mkdir_test, "", "The directory for mkdir test.");
ABSL_FLAG(std::string, createfile_test, "", "The filename for createfile test.");
ABSL_FLAG(std::string, rmfile_test, "", "The filename for rmfile test.");
ABSL_FLAG(std::string, rmdir_test, "", "The filename for rmdir test.");
ABSL_FLAG(std::string, rmdir_recursive_test, "", "The filename for rmdir_recursive test.");
ABSL_FLAG(std::string, write_test, "", "The filename for write_test test.");
ABSL_FLAG(std::string, read_test, "", "The filename for read_test test.");

int main(int argc, char* argv[]) {
    absl::ParseCommandLine(argc, argv);
    google::InitGoogleLogging(argv[0]);
    auto fs_instance = galaxy::GalaxyFs::Instance();
    if (!absl::GetFlag(FLAGS_mkdir_test).empty()) {
        fs_instance->CreateDirIfNotExist(absl::GetFlag(FLAGS_mkdir_test));
    }
    if (!absl::GetFlag(FLAGS_createfile_test).empty()) {
        fs_instance->CreateFileIfNotExist(absl::GetFlag(FLAGS_createfile_test));
    }
    if (!absl::GetFlag(FLAGS_rmfile_test).empty()) {
        fs_instance->RmFile(absl::GetFlag(FLAGS_rmfile_test));
    }
    if (!absl::GetFlag(FLAGS_rmdir_test).empty()) {
        fs_instance->RmDir(absl::GetFlag(FLAGS_rmdir_test));
    }
    if (!absl::GetFlag(FLAGS_rmdir_recursive_test).empty()) {
        fs_instance->RmDirRecursive(absl::GetFlag(FLAGS_rmdir_recursive_test));
    }
    if (!absl::GetFlag(FLAGS_write_test).empty()) {
        fs_instance->Write(absl::GetFlag(FLAGS_write_test), "hello world", "w");
    }
    if (!absl::GetFlag(FLAGS_read_test).empty()) {
        std::string data;
        fs_instance->Read(absl::GetFlag(FLAGS_read_test), data);
        std::cout << "Read test: " << data << std::endl;
    }
    return 0;
}