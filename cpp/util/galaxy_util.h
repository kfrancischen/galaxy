#ifndef CPP_UTIL_GALAXY_UTIL_H_
#define CPP_UTIL_GALAXY_UTIL_H_

#include "absl/status/statusor.h"
#include "schema/fileserver.pb.h"
#include <vector>

namespace galaxy {
    namespace util {
        absl::StatusOr<std::string> ParseGlobalConfig(bool is_server=true, const std::string& cell="");

        absl::StatusOr<std::vector<std::string>> ParseGlobalConfigAndGetCells();

        absl::StatusOr<std::pair<std::string, std::string>> GetCellAndPathFromPath(const std::string& path);

        absl::StatusOr<std::string> InitClient(const std::string& path);

        std::string ConvertToCellPath(const std::string& path);


        absl::StatusOr<std::string> ConvertToLocalPath(const std::string& path);

        // New Galaxy Util APIs
        absl::StatusOr<galaxy_schema::FileAnalyzerResult> RunFileAnalyzer(const std::string& path);
        absl::StatusOr<galaxy_schema::CellConfig> ParseCellConfig(const std::string& cell);
        galaxy_schema::FileAnalyzerResult InitClientV2(const std::string& path);
    }  // namespace util

} // namespace galaxy

#endif  // CPP_UTIL_GALAXY_UTIL_H_