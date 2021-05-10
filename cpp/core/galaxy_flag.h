#ifndef CPP_CORE_GALAX__FLAG_H_
#define CPP_CORE_GALAXY_FLAG_H_
#include "absl/flags/declare.h"

ABSL_DECLARE_FLAG(std::string, fs_root);
ABSL_DECLARE_FLAG(std::string, fs_address);
ABSL_DECLARE_FLAG(std::string, fs_password);
ABSL_DECLARE_FLAG(int, fs_log_ttl);
ABSL_DECLARE_FLAG(std::string, fs_log_dir);
ABSL_DECLARE_FLAG(int, fs_verbose_level);
ABSL_DECLARE_FLAG(bool, fs_alsologtostderr);
ABSL_DECLARE_FLAG(int, fs_rpc_ddl);
ABSL_DECLARE_FLAG(int, fs_num_thread);
ABSL_DECLARE_FLAG(int, fs_max_msg_size);

// Global configurations
ABSL_DECLARE_FLAG(std::string, fs_global_config);
ABSL_DECLARE_FLAG(std::string, fs_cell);

#endif  // CPP_CORE_GALAXY_FLAG_H_
