#ifndef CPP_CORE_GALAXY_FLAG_H_
#define CPP_CORE_GALAXY_FLAG_H_
#include "absl/flags/declare.h"

ABSL_DECLARE_FLAG(int, fs_rpc_ddl);

// Global configurations
ABSL_DECLARE_FLAG(std::string, fs_global_config);
ABSL_DECLARE_FLAG(std::string, fs_cell);

#endif  // CPP_CORE_GALAXY_FLAG_H_
