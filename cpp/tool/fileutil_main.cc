#include <string>
#include "cpp/tool/fileutil.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/util/galaxy_util.h"
#include "glog/logging.h"
#include "absl/flags/flag.h"

int main(int argc, char* argv[]) {
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false, "");
    CHECK(result.ok()) << "Fail to parse the global config.";
    FLAGS_colorlogtostderr = true;
    FLAGS_log_dir = absl::GetFlag(FLAGS_fs_log_dir);
    FLAGS_alsologtostderr = absl::GetFlag(FLAGS_fs_alsologtostderr);
    FLAGS_stderrthreshold = 3;
    google::EnableLogCleaner(absl::GetFlag(FLAGS_fs_log_ttl));
    google::InitGoogleLogging(argv[0]);
    CHECK_GT(argc, 1) << "Need more than 1 arguments";
    LOG(INFO) << "Getting cmd:";
    for (int i = 1; i < argc; i++) {
        LOG(INFO) << "\tArg[" << i << "]: " << argv[i];
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
        CHECK_GE(argc,  4) << "Need at least 3 arguments for cp cmd.";
        bool overwrite = false;
        if (argc == 5 && strcmp(argv[4], "--f") == 0) {
            overwrite = true;
        }
        galaxy::CopyCmd(argv[2], argv[3], overwrite);
    } else if (strcmp(argv[1], "mv") == 0) {
        CHECK_GE(argc,  4) << "Need at least 3 arguments for mv cmd.";
        bool overwrite = false;
        if (argc == 5 && strcmp(argv[4], "--f") == 0) {
            overwrite = true;
        }
        galaxy::MoveCmd(argv[2], argv[3], overwrite);
    } else if (strcmp(argv[1], "rm") == 0) {
        CHECK_GE(argc,  3) << "Need at least 2 arguments for cp cmd.";
        bool recursive = false;
        if (argc == 4 && strcmp(argv[3], "--r") == 0) {
            recursive = true;
        }
        galaxy::RmCmd(argv[2], recursive);
    } else if (strcmp(argv[1], "lscells") == 0) {
        galaxy::ListCellsCmd();
    } else {
        LOG(FATAL) << "Wrong cmd.";
    }
}
