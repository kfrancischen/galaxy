#include "absl/flags/flag.h"
#include "cpp/core/galaxy_flag.h"

ABSL_FLAG(std::string, fs_root, "", "The root path of the file system.");
ABSL_FLAG(std::string, fs_address, "", "The address of the file system.");
ABSL_FLAG(std::string, fs_password, "", "The password contained in gRPC.");
ABSL_FLAG(int, fs_log_ttl, 7, "The ttl for log files.");
ABSL_FLAG(std::string, fs_log_dir, "/home/pslx/Downloads/galaxy_log", "The directory for log files.");
ABSL_FLAG(int, fs_verbose_level, 0, "The verbose level for glogs.");
ABSL_FLAG(bool, fs_alsologtostderr, false, "Whether to also send log to stderr in addition to log files.");

// Global configurations
ABSL_FLAG(std::string, fs_global_config, "", "The global configuration (json file) for galaxy filesystems.");
ABSL_FLAG(std::string, fs_cell, "", "Current cell of the galaxy filesystems.");
