#include "ext/ttl_cleaner/galaxy_ttl_cleaner.h"
#include <time.h>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/time/clock.h"

#include "cpp/client.h"
#include "cpp/util/galaxy_util.h"
#include "cpp/core/galaxy_flag.h"
#include "glog/logging.h"

ABSL_FLAG(int, run_every, 10, "Interval (in minutes) to run the ttl cleaner.");

struct TTLStat {
    int num_file_removed;
    int num_dir_removed;
};


void RunTTLCleanerOverDirectoryFiles(const std::string& path, TTLStat& ttl_stat) {
    if (path.find(galaxy::ext::kExcludedDir) != std::string::npos) {
        return;
    }
    std::string ttl = galaxy::ext::GetTTLFromPath(path);
    std::time_t path_m_time = galaxy::ext::GetFileModifiedTime(path);
    LOG(INFO) << "TTL for path " << path << " is " << ttl;
    std::time_t cur_time = galaxy::ext::GetCurrentTime();
    double ttl_time = galaxy::ext::GetTTLTime(ttl);
    if (ttl_time > 0) {
        std::map<std::string, std::string> files = galaxy::client::ListFilesInDir(path, true);
        for (const auto & file : files) {
            std::time_t m_time = galaxy::ext::GetFileModifiedTime(file.first);
            if (m_time > 0 && difftime(cur_time, m_time) >= ttl_time) {
                VLOG(1) << "Removing file " << file.first;
                galaxy::client::RmFile(file.first, true);
                ttl_stat.num_file_removed += 1;
            }
        }
    }
    std::map<std::string, std::string> dirs = galaxy::client::ListDirsInDir(path);

    for (const auto& dir : dirs) {
        RunTTLCleanerOverDirectoryFiles(dir.first, ttl_stat);
    }

    // The dir is empty after removing everything under it.
    if (ttl_time > 0 && galaxy::client::ListFilesInDir(path).size() + galaxy::client::ListDirsInDir(path).size() == 0) {
        if (difftime(cur_time, path_m_time) >= ttl_time) {
            ttl_stat.num_dir_removed += 1;
            VLOG(1) << "Removing directory " << path;
            galaxy::client::RmDir(path, true);
        }
    }
    return;
}


void RunTTLCleaner(const std::string& root_path) {
    LOG(INFO) << "TTL cleaner started at " << galaxy::ext::GetCurrentTime();
    TTLStat ttl_stat;
    ttl_stat.num_file_removed = 0;
    ttl_stat.num_dir_removed = 0;
    RunTTLCleanerOverDirectoryFiles(root_path, ttl_stat);
    LOG(INFO) << "Number of files deleted: " << ttl_stat.num_file_removed;
    LOG(INFO) << "Number of dirs deleted: " << ttl_stat.num_dir_removed;
}

int main(int argc, char* argv[])
{
    absl::ParseCommandLine(argc, argv);
    FLAGS_colorlogtostderr = true;
    absl::StatusOr<galaxy_schema::CellConfig> config = galaxy::util::ParseCellConfig(absl::GetFlag(FLAGS_fs_cell));
    CHECK(config.ok()) << "Fail to parse the cell config.";
    FLAGS_v = config->fs_verbose_level();
    FLAGS_log_dir = config->fs_log_dir();
    FLAGS_max_log_size = 10;  // setting the maximum log size to 10M
    FLAGS_alsologtostderr = config->fs_alsologtostderr();
    google::EnableLogCleaner(config->fs_log_ttl());
    google::InitGoogleLogging(argv[0]);
    while (true) {
        RunTTLCleaner(config->fs_root());
        absl::SleepFor(absl::Minutes(absl::GetFlag(FLAGS_run_every)));
    }
    return 0;
}
