#include "absl/flags/flag.h"
#include "cpp/core/galaxy_flag.h"

ABSL_FLAG(std::string, fs_root, "", "The root path of the file system.");
ABSL_FLAG(std::string, fs_address, "", "The address of the file system.");
