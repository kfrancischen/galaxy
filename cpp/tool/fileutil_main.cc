#include <string>
#include "cpp/tool/fileutil.h"
#include "cpp/core/galaxy_flag.h"
#include "glog/logging.h"
#include "absl/flags/flag.h"

int main(int argc, char* argv[]) {
    FLAGS_colorlogtostderr = true;
    FLAGS_log_dir = absl::GetFlag(FLAGS_fs_log_dir);
    FLAGS_alsologtostderr = true;
    google::EnableLogCleaner(absl::GetFlag(FLAGS_fs_log_ttl));
    google::InitGoogleLogging(argv[0]);
    CHECK_GT(argc, 2) << "Need more than 1 arguments";
    LOG(INFO) << "Getting cmd:";
    for (int i = 1; i < argc; i++) {
        LOG(INFO) << "\t--" << argv[i];
    }
    if (strcmp(argv[1], "get") == 0) {
        CHECK_EQ(argc,  4) << "Need 3 arguments for get cmd.";
        galaxy::GetFileCmd(argv[2], argv[3]);
    } else if (strcmp(argv[1], "upload") == 0) {
        CHECK_EQ(argc,  4) << "Need 3 arguments for upload cmd.";
        galaxy::UploadFileCmd(argv[2], argv[3]);
    } else if (strcmp(argv[1], "ls") == 0) {
        CHECK_EQ(argc,  3) << "Need 2 arguments for ls cmd.";
        galaxy::LsCmd(argv[2]);
    } else if (strcmp(argv[1], "cp") == 0) {
        CHECK_EQ(argc,  4) << "Need 3 arguments for cp cmd.";
        galaxy::CopyFileCmd(argv[2], argv[3]);
    } else if (strcmp(argv[1], "mv") == 0) {
        CHECK_EQ(argc,  4) << "Need 3 arguments for mv cmd.";
        galaxy::MoveFileCmd(argv[2], argv[3]);
    } else {
        LOG(FATAL) << "Wrong cmd.";
    }

}