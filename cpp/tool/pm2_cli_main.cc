#include <string>
#include "cpp/tool/pm2_cli.h"
#include "cpp/remote_execution.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/util/galaxy_util.h"
#include "glog/logging.h"
#include "absl/flags/flag.h"

int main(int argc, char* argv[]) {
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    FLAGS_colorlogtostderr = true;
    FLAGS_log_dir = absl::GetFlag(FLAGS_fs_log_dir);
    FLAGS_alsologtostderr = absl::GetFlag(FLAGS_fs_alsologtostderr);
    FLAGS_stderrthreshold = 3;
    google::EnableLogCleaner(absl::GetFlag(FLAGS_fs_log_ttl));
    google::InitGoogleLogging(argv[0]);
    CHECK_GT(argc, 2) << "Need more than 2 arguments";
    LOG(INFO) << "Getting cmd:";
    for (int i = 1; i < argc; i++) {
        LOG(INFO) << "\tArg[" << i << "]: " << argv[i];
    }
    if (strcmp(argv[1], "list") == 0) {
        CHECK_LE(argc, 3) << "No more than 2 arguments in list.";
        if (argc == 2) {
            galaxy::Pm2List();
        } else {
            std::string cell(argv[2]);
            galaxy::Pm2List(cell);
        }
    } else if (strcmp(argv[1], "start") == 0) {
        CHECK_EQ(argc, 6) << "Need 6 arguments in start.";
        std::string cell(argv[2]);
        std::string home_dir(argv[3]);
        std::string json_file(argv[4]);
        std::string job_name(argv[5]);
        galaxy::Pm2Start(cell, home_dir, json_file, job_name);
    } else if (strcmp(argv[1], "stop") == 0) {
        CHECK_GE(argc, 3) << "No less than 1 arguments in stop.";
        CHECK_LE(argc, 4) << "No more than 2 arguments in stop.";
        if (argc == 3) {
            std::string job_name(argv[2]);
            galaxy::Pm2Stop(job_name);
        } else {
            std::string cell(argv[2]);
            std::string job_name(argv[3]);
            galaxy::Pm2Stop(cell, job_name);
        }
    } else if (strcmp(argv[1], "restart") == 0) {
        CHECK_GE(argc, 3) << "No less than 1 arguments in restart.";
        CHECK_LE(argc, 4) << "No more than 2 arguments in restart.";
        if (argc == 3) {
            std::string job_name(argv[2]);
            galaxy::Pm2Restart(job_name);
        } else {
            std::string cell(argv[2]);
            std::string job_name(argv[3]);
            galaxy::Pm2Restart(cell, job_name);
        }
    } else {
        LOG(FATAL) << "Wrong cmd.";
    }
}
