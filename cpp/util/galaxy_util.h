#ifndef CPP_UTIL_GALAXY_UTIL_H_
#define CPP_UTIL_GALAXY_UTIL_H_

#include "absl/status/statusor.h"

namespace galaxy {
    namespace util {
        absl::StatusOr<std::string> ParseGlobalConfig(bool is_server=true);

        absl::StatusOr<std::pair<std::string, std::string>> GetCellAndPathFromPath(const std::string& path);
    }  // namespace util

} // namespace galaxy

#endif  // CPP_UTIL_GALAXY_UTIL_H_