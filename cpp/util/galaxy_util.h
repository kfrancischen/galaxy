#ifndef CPP_UTIL_GALAXY_UTIL_H_
#define CPP_UTIL_GALAXY_UTIL_H_

#include <vector>

#include "absl/status/statusor.h"
#include "schema/fileserver.pb.h"

namespace galaxy {
    namespace util {
        // New Galaxy Util APIs
        std::vector<std::string> GetAllCells();
        std::string GetGalaxyFsPrefixPath(const std::string& cell);
        std::vector<std::string> BroadcastSharedPath(const std::string& path, const std::vector<std::string>& cells);
        absl::StatusOr<galaxy_schema::FileAnalyzerResult> RunFileAnalyzer(const std::string& path);
        absl::StatusOr<galaxy_schema::CellConfig> ParseCellConfig(const std::string& cell);
        bool IsLocalPath(const std::string& path);
        galaxy_schema::FileAnalyzerResult InitClient(const std::string& path);
        std::string ConvertToCellPath(const std::string& path, const galaxy_schema::CellConfig& config);
    }  // namespace util

} // namespace galaxy

#endif  // CPP_UTIL_GALAXY_UTIL_H_