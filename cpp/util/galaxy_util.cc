#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "absl/flags/flag.h"
#include "absl/status/status.h"
#include "absl/strings/str_split.h"
#include "absl/strings/str_join.h"
#include "cpp/util/galaxy_util.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/internal/galaxy_const.h"
#include "glog/logging.h"
#include "include/rapidjson/istreamwrapper.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/prettywriter.h"
#include "include/rapidjson/stringbuffer.h"

using galaxy_schema::FileAnalyzerResult;
using galaxy_schema::CellConfig;

rapidjson::Document ParseCellsConfigDoc() {
    std::string config_path = absl::GetFlag(FLAGS_fs_global_config);
    std::ifstream infile;
    infile.open(config_path);
    if (infile) {
        rapidjson::IStreamWrapper isw(infile);
        rapidjson::Document doc;
        if (doc.ParseStream(isw).HasParseError()) {
            infile.close();
            throw "Configuration json cannot be parsed.";
        }
        infile.close();
        return doc;
    }
    throw "Configuration json cannot be found";
}


std::string galaxy::util::GetGalaxyFsPrefixPath(const std::string& cell) {
    std::string separator(1, galaxy::constant::kSeparator);
    std::string cell_suffix(galaxy::constant::kCellSuffix);
    std::string cell_prefix(galaxy::constant::kCellPrefix);
    return cell_prefix + separator + cell + cell_suffix;
}

std::string galaxy::util::ConvertToCellPath(const std::string& path, const CellConfig& config) {
    std::string cell_name = config.cell();
    // Not a remote call.
    if (cell_name.empty()) {
        return path;
    }
    std::string fs_local = config.fs_root();
    // Not a path in galaxy.
    if (path.find(fs_local) == std::string::npos) {
        return path;
    }
    std::string path_prefix = galaxy::util::GetGalaxyFsPrefixPath(cell_name);
    std::string out_path(path);
    out_path.replace(0, fs_local.length(), path_prefix);
    return out_path;
}

std::string NormalizeDir(const std::string& dir) {
    std::string output_dir(dir);
    if (!output_dir.empty() && output_dir[-1] == '/') {
        output_dir.pop_back();
    }
    return output_dir;
}

absl::StatusOr<CellConfig> ParseCellConfigInternal(const rapidjson::Value& cell_config) {
    if (!cell_config.HasMember("fs_root") || !cell_config.HasMember("fs_ip") || !cell_config.HasMember("fs_port") || !cell_config.HasMember("fs_password") || !cell_config.HasMember("fs_stats_port")) {
        return absl::FailedPreconditionError("Imcomplete configuration file. The file should at least contain fs_root, fs_ip, fs_port, fs_stats_port and fs_password.");
    }
    CellConfig config;
    config.set_fs_root(NormalizeDir(cell_config["fs_root"].GetString()));
    config.set_fs_password(cell_config["fs_password"].GetString());

    config.set_fs_port(cell_config["fs_port"].GetInt());
    config.set_fs_stats_port(cell_config["fs_stats_port"].GetInt());
    config.set_fs_ip(cell_config["fs_ip"].GetString());

    if (cell_config.HasMember("fs_log_dir")) {
        std::string log_dir = NormalizeDir(cell_config["fs_log_dir"].GetString());
        mkdir(log_dir.c_str(), 0777);
        config.set_fs_log_dir(log_dir);
    }
    if (cell_config.HasMember("fs_log_ttl")) {
        config.set_fs_log_ttl(cell_config["fs_log_ttl"].GetInt());
    } else {
        config.set_fs_log_ttl(7);
    }

    if (cell_config.HasMember("fs_verbose_level")) {
        config.set_fs_verbose_level(cell_config["fs_verbose_level"].GetInt());
    } else {
        config.set_fs_verbose_level(0);
    }

    if (cell_config.HasMember("fs_alsologtostderr")) {
        config.set_fs_alsologtostderr(cell_config["fs_alsologtostderr"].GetBool());
    }

    if (cell_config.HasMember("fs_num_thread")) {
        config.set_fs_num_thread(cell_config["fs_num_thread"].GetInt());
    } else {
        config.set_fs_num_thread(3);
    }

    if (cell_config.HasMember("fs_max_msg_size")) {
        config.set_fs_max_msg_size(cell_config["fs_max_msg_size"].GetInt());
    } else {
        config.set_fs_max_msg_size(40);
    }

    if (cell_config.HasMember("disabled")) {
        config.set_disabled(cell_config["disabled"].GetBool());
    } else {
        config.set_disabled(false);
    }
    return config;
}

std::vector<std::string> galaxy::util::GetAllCells() {
    rapidjson::Document cells_config = ParseCellsConfigDoc();
    std::vector<std::string> cells;
    for (auto it = cells_config.MemberBegin(); it != cells_config.MemberEnd(); it++) {
        const std::string& cell_name = it->name.GetString();
        auto config = ParseCellConfigInternal(cells_config[cell_name.c_str()]);
        if (!config.ok() || config->disabled()) {
            continue;
        }
        cells.push_back(cell_name);
    }
    return cells;
}

absl::StatusOr<CellConfig> galaxy::util::ParseCellConfig(const std::string& cell) {
    rapidjson::Document cells_config = ParseCellsConfigDoc();
    if (!cells_config.HasMember(cell.c_str())) {
        return absl::InvalidArgumentError("Configuration cannot be found for cell [" + cell + "].");
    }
    auto config = ParseCellConfigInternal(cells_config[cell.c_str()]);
    if (!config.ok()) {
        return config.status();
    } else {
        config->set_cell(cell);
        return config;
    }
}

absl::StatusOr<FileAnalyzerResult> galaxy::util::RunFileAnalyzer(const std::string& path) {
    FileAnalyzerResult result;
    std::string output_path(path);
    rapidjson::Document cells_config = ParseCellsConfigDoc();
    std::string local_prefix(galaxy::constant::kLocalPrefix);
    std::string shared_prefix(galaxy::constant::kSharedPrefix);

    std::string from_cell = absl::GetFlag(FLAGS_fs_cell);
    // If the request is not from a cell in galaxy, reformat it if necessary.
    if (from_cell.empty()) {
        char* fs_root_char = getenv("GALAXY_fs_root");
        if (fs_root_char != NULL) {
            // It is a path started with /SHARED.
            if (output_path.find(shared_prefix) != std::string::npos) {
                result.set_is_shared(true);
            } else if (output_path.find(local_prefix) != std::string::npos) {
                // It is a path started with /LOCAL but not in the galaxy system.
                output_path.replace(0, local_prefix.length(), NormalizeDir(fs_root_char));
            }
        }
    } else {  // If the request is from a cell, validate the cell config.
        if (!cells_config.HasMember(from_cell.c_str())) {
            return absl::InternalError("Configuration cannot be found for cell [" + from_cell + "].");
        }
        const rapidjson::Value& cell_config = cells_config[from_cell.c_str()];
        result.set_from_cell(from_cell);
        result.set_to_cell(from_cell);
        std::string path_prefix = galaxy::util::GetGalaxyFsPrefixPath(from_cell);  // /galaxy/{cell_name}-d
        std::string from_cell_fs_root = NormalizeDir(cell_config["fs_root"].GetString());

        // Case 1: the request path starts with /LOCAL
        if (output_path.find(local_prefix) != std::string::npos) {
            // Output path trims the prefixing /LOCAL and replaces it with fs_root.
            output_path.replace(0, local_prefix.length(), from_cell_fs_root);
        } else if (output_path.find(shared_prefix) != std::string::npos) {
            // Case 2: the request path starts with /SHARED
            // Keep the original path here.
            result.set_is_shared(true);
        } else if (output_path.find(path_prefix) != std::string::npos) {
            // Case 3: the path is in the format of /galaxy/CELL-d/.., where CELL is the current server cell.
            // Output path trims the prefixing and repalces it wwith fs_root.
            output_path.replace(0, path_prefix.length(), from_cell_fs_root);
        }
    }

    // Case 3: the path is in the format of /.... and it is definitely not a local path.
    if (!output_path.empty() && output_path[0] == galaxy::constant::kSeparator) {
        std::vector<std::string> v = absl::StrSplit(output_path, galaxy::constant::kSeparator);
        std::string cell_suffix(galaxy::constant::kCellSuffix);
        std::string cell_prefix(galaxy::constant::kCellPrefix);
        std::string separator(1, galaxy::constant::kSeparator);
        // case 3.1: a remote path.
        if (v.size() >= 3 && separator + v[1] == cell_prefix && v[2].find(cell_suffix) != std::string::npos) {
            std::string to_cell = v[2];
            to_cell.erase(to_cell.end() - cell_suffix.length(), to_cell.end());
            if (!cells_config.HasMember(to_cell.c_str())) {
                return absl::InternalError("Configuration cannot be found for cell [" + from_cell + "].");
            }
            result.set_to_cell(to_cell);
            v.erase(v.begin(), v.begin() + 3);
            std::string to_cell_fs_root = NormalizeDir(cells_config[to_cell.c_str()]["fs_root"].GetString());
            v.insert(v.begin(), to_cell_fs_root);
            std::string file_path = absl::StrJoin(v, separator);
            output_path = file_path;
            result.set_is_remote(true);
        }
    }
    if (!result.from_cell().empty()) {
        absl::StatusOr<CellConfig> from_cell_config = ParseCellConfigInternal(cells_config[result.from_cell().c_str()]);
        if (!from_cell_config.ok()) {
            return from_cell_config.status();
        } else {
            from_cell_config->set_cell(result.from_cell());
            *result.mutable_configs()->mutable_from_cell_config() = *from_cell_config;
        }
    }
    result.set_path(output_path);
    if (!result.to_cell().empty()) {
        absl::StatusOr<CellConfig> to_cell_config = ParseCellConfigInternal(cells_config[result.to_cell().c_str()]);
        if (!to_cell_config.ok()) {
            return to_cell_config.status();
        } else {
            to_cell_config->set_cell(result.to_cell());
            *result.mutable_configs()->mutable_to_cell_config() = *to_cell_config;
        }
    }
    return result;
}

bool galaxy::util::IsLocalPath(const std::string& path) {
    auto result = galaxy::util::RunFileAnalyzer(path);
    if (!result.ok()) {
        return true;
    } else {
        return !result->is_remote();
    }
}

FileAnalyzerResult galaxy::util::InitClient(const std::string& path) {
    absl::StatusOr<FileAnalyzerResult> result = galaxy::util::RunFileAnalyzer(path);
    CHECK(result.ok()) << result.status();
    CHECK(!result->configs().from_cell_config().disabled()) << "Cell [" + result->from_cell() + "] is disabled";
    CHECK(!result->configs().to_cell_config().disabled()) << "Cell [" + result->to_cell() + "] is disabled";
    return *result;
}

std::vector<std::string> galaxy::util::BroadcastSharedPath(const std::string &path, const std::vector<std::string> &cells)
{
    std::string shared_prefix(galaxy::constant::kSharedPrefix);
    CHECK(path.find(shared_prefix) != std::string::npos) << "Path needs to have /SHARED as the prefix.";
    std::vector<std::string> output_paths;
    if (cells.empty())
    {
        std::string local_prefix(galaxy::constant::kLocalPrefix);
        std::string out_path(path);
        out_path.replace(0, shared_prefix.length(), local_prefix);
        output_paths.push_back(out_path);
    }
    else
    {
        for (const auto &cell : cells)
        {
            std::string path_prefix = galaxy::util::GetGalaxyFsPrefixPath(cell);
            std::string out_path(path);
            out_path.replace(0, shared_prefix.length(), path_prefix);
            output_paths.push_back(out_path);
        }
    }
    return output_paths;
}
