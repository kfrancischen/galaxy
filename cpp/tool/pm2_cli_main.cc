#include <string>
#include "absl/flags/flag.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/tool/pm2_cli.h"
#include "cpp/util/galaxy_util.h"
#include "glog/logging.h"

int main(int argc, char* argv[]) {
    absl::StatusOr<galaxy_schema::CellConfig> config = galaxy::util::ParseCellConfig(absl::GetFlag(FLAGS_fs_cell));
    if (config.ok()) {
        FLAGS_colorlogtostderr = true;
        FLAGS_log_dir = config->fs_log_dir();;
        FLAGS_alsologtostderr = config->fs_alsologtostderr();
        FLAGS_stderrthreshold = 3;
        google::EnableLogCleaner(config->fs_log_ttl());
    } else {
        LOG(WARNING) << "Calling from a server outside galaxy.";
    }
    google::InitGoogleLogging(argv[0]);
    CHECK_GE(argc, 2) << "Need no less than 2 arguments";
    LOG(INFO) << "Getting cmd:";
    for (int i = 1; i < argc; i++) {
        LOG(INFO) << "\tArg[" << i << "]: " << argv[i];
    }
    if (strcmp(argv[1], "list") == 0 || strcmp(argv[1], "ls") == 0) {
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
