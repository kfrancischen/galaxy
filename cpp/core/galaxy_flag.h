#ifndef CPP_CORE_GALAX__FLAG_H_
#define CPP_CORE_GALAXY_FLAG_H_
#include "absl/flags/declare.h"

ABSL_DECLARE_FLAG(std::string, fs_root);
ABSL_DECLARE_FLAG(std::string, fs_address);
ABSL_DECLARE_FLAG(std::string, fs_password);
ABSL_DECLARE_FLAG(int, fs_log_ttl);
ABSL_DECLARE_FLAG(std::string, fs_log_dir);
ABSL_DECLARE_FLAG(int, fs_verbose_level);

#endif  // CPP_CORE_GALAXY_FLAG_H_
