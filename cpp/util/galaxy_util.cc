#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "cpp/util/galaxy_util.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/internal/galaxy_const.h"
#include "include/rapidjson/istreamwrapper.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/prettywriter.h"
#include "include/rapidjson/stringbuffer.h"
#include "absl/flags/flag.h"
#include "absl/status/status.h"
#include "absl/strings/str_split.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"

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

std::string GetGalaxyFsPath(const std::string& cell) {
    std::string separator(1, galaxy::constant::kSeparator);
    std::string cell_suffix(galaxy::constant::kCellSuffix);
    std::string cell_prefix(galaxy::constant::kCellPrefix);
    return cell_prefix + separator + cell + cell_suffix;
}


absl::StatusOr<std::string> galaxy::util::ParseGlobalConfig(bool is_server, const std::string& cell) {
    rapidjson::Document cells_config = ParseCellsConfigDoc();
    std::string cell_name;
    if (cell.empty()) {
        cell_name = absl::GetFlag(FLAGS_fs_cell);
    } else {
        cell_name = cell;
    }

    if (!cells_config.HasMember(cell_name.c_str())) {
        return absl::InternalError("Cell configuration cannot be found.");
    }
    const rapidjson::Value& cell_config = cells_config[cell_name.c_str()];
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    cell_config.Accept(writer);
    if (!cell_config.HasMember("fs_root") || !cell_config.HasMember("fs_ip") || !cell_config.HasMember("fs_port") || !cell_config.HasMember("fs_password") || !cell_config.HasMember("fs_stats_port")) {
        return absl::FailedPreconditionError("Imcomplete configuration file. The file should at least contain fs_root, fs_ip, fs_port, fs_stats_port and fs_password.");
    }
    absl::SetFlag(&FLAGS_fs_root, cell_config["fs_root"].GetString());
    absl::SetFlag(&FLAGS_fs_password, cell_config["fs_password"].GetString());
    std::string fs_ip = cell_config["fs_ip"].GetString();
    int fs_port = cell_config["fs_port"].GetInt();
    int fs_stats_port = cell_config["fs_stats_port"].GetInt();
    if (is_server) {
        absl::SetFlag(&FLAGS_fs_address, "0.0.0.0:" + std::to_string(fs_port));
        absl::SetFlag(&FLAGS_fs_stats_address, "0.0.0.0:" + std::to_string(fs_stats_port));
    } else {
        absl::SetFlag(&FLAGS_fs_address, fs_ip + ":" + std::to_string(fs_port));
    }

    if (cell_config.HasMember("fs_log_ttl")) {
        absl::SetFlag(&FLAGS_fs_log_ttl, cell_config["fs_log_ttl"].GetInt());
    }
    if (cell_config.HasMember("fs_log_dir")) {
        std::string log_dir = cell_config["fs_log_dir"].GetString();
        mkdir(log_dir.c_str(), 0777);
        absl::SetFlag(&FLAGS_fs_log_dir, log_dir);
    }
    if (cell_config.HasMember("fs_verbose_level")) {
        absl::SetFlag(&FLAGS_fs_verbose_level, cell_config["fs_verbose_level"].GetInt());
    }
    if (cell_config.HasMember("fs_alsologtostderr")) {
        absl::SetFlag(&FLAGS_fs_alsologtostderr, cell_config["fs_alsologtostderr"].GetBool());
    }
    if (cell_config.HasMember("fs_num_thread") && is_server) {
        absl::SetFlag(&FLAGS_fs_num_thread, cell_config["fs_num_thread"].GetInt());
    }
    if (cell_config.HasMember("fs_max_msg_size") && is_server) {
        absl::SetFlag(&FLAGS_fs_max_msg_size, cell_config["fs_max_msg_size"].GetInt());
    }
    return "Getting cell config for cell [" + cell_name + "]:\n" + sb.GetString();
}

absl::StatusOr<std::pair<std::string, std::string>> galaxy::util::GetCellAndPathFromPath(const std::string& path) {
    // This function only returns a <cell, path> pair when the path is remote.
    // Otherwise a status error will be thrown.
    if (path.empty() || path[0] != galaxy::constant::kSeparator) {
        return absl::InvalidArgumentError("Input path is invalid.");
    }
    std::string path_copy(path);
    path_copy.erase(path_copy.begin());
    std::vector<std::string> v = absl::StrSplit(path_copy, galaxy::constant::kSeparator);
    std::string cell_suffix(galaxy::constant::kCellSuffix);
    std::string cell_prefix(galaxy::constant::kCellPrefix);
    std::string separator(1, galaxy::constant::kSeparator);
    if (v.size() < 2 || separator + v[0] != cell_prefix || v[1].find(cell_suffix) == std::string::npos) {
        return absl::InvalidArgumentError("Input path is invalid.");
    }
    std::string cell = v[1];
    cell.erase(cell_suffix.length());
    char* cell_name_char = getenv("GALAXY_fs_cell");
    if (cell_name_char != NULL) {
        std::string cell_name(cell_name_char);
        // The requested cell is the same cell as the requester.
        if (cell_name == cell) {
            return absl::InvalidArgumentError("Input path is in galaxy but not a remote path.");
        }
    }

    v.erase(v.begin(), v.begin() + 2);
    std::string file_path = absl::StrJoin(v, separator);
    return std::make_pair(cell, file_path);
}


absl::StatusOr<std::string> galaxy::util::InitClient(const std::string& path) {
    absl::StatusOr<std::pair<std::string, std::string>> cell_and_path = galaxy::util::GetCellAndPathFromPath(path);
    if (!cell_and_path.ok()) {
        // For local path, set the cell to be empty.
        absl::SetFlag(&FLAGS_fs_cell, "");
        return absl::InternalError("Wrong format of path, or local path.");
    } else {
        // For remote path, set the cell to the one inferred from the path.
        absl::SetFlag(&FLAGS_fs_cell, (*cell_and_path).first);
        return (*cell_and_path).second;
    }
}

absl::StatusOr<std::vector<std::string>> galaxy::util::ParseGlobalConfigAndGetCells() {
    rapidjson::Document cells_config = ParseCellsConfigDoc();
    std::vector<std::string> cells;
    for (auto it = cells_config.MemberBegin(); it != cells_config.MemberEnd(); it++) {
        cells.push_back(it->name.GetString());
    }
    return cells;
}


absl::StatusOr<std::string> galaxy::util::ConvertToLocalPath(const std::string& path) {
    rapidjson::Document cells_config = ParseCellsConfigDoc();
    std::string output_path(path);
    std::string local_prefix(galaxy::constant::kLocalPrefix);
    char* cell_name_char = getenv("GALAXY_fs_cell");
    if (cell_name_char == NULL) {
        // It is a computer not in the galaxy system.
        char* fs_root_char = getenv("GALAXY_fs_root");
        if (fs_root_char != NULL && output_path.find(local_prefix) != std::string::npos) {
            output_path.replace(0, local_prefix.length(), fs_root_char);
        }
        return output_path;
    }
    std::string cell_name(cell_name_char);
    if (!cells_config.HasMember(cell_name.c_str())) {
        return absl::InternalError("Cell configuration cannot be found.");
    }
    const rapidjson::Value& cell_config = cells_config[cell_name.c_str()];
    if (!cell_config.HasMember("fs_root")) {
        return absl::FailedPreconditionError("Imcomplete configuration file. The file should at least contain fs_root.");
    }
    std::string path_prefix = GetGalaxyFsPath(cell_name);

    if (output_path.find(local_prefix) != std::string::npos) {
        // This occurs when path is in the format of /LOCAL/...
        output_path.replace(0, local_prefix.length(), cell_config["fs_root"].GetString());
    } else if (output_path.find(path_prefix) != std::string::npos) {
        // This occurs when the path is in the format of /galaxy/CELL-d/.., where CELL is the current server cell.
        output_path.replace(0, path_prefix.length(), cell_config["fs_root"].GetString());
    }
    return output_path;
}


std::string galaxy::util::ConvertToCellPath(const std::string& path) {
    std::string cell_name = absl::GetFlag(FLAGS_fs_cell);
    // Not a remote call.
    if (cell_name.empty()) {
        char* cell_name_char = getenv("GALAXY_fs_cell");
        // Not a cell in the galaxy.
        if (cell_name_char == NULL) {
            return path;
        }
        cell_name = cell_name_char;
    }
    rapidjson::Document cells_config = ParseCellsConfigDoc();
    const rapidjson::Value& cell_config = cells_config[cell_name.c_str()];
    std::string fs_local = cell_config["fs_root"].GetString();
    // Not a path in galaxy.
    if (path.find(fs_local) == std::string::npos) {
        return path;
    }
    std::string path_prefix = GetGalaxyFsPath(cell_name);
    std::string out_path(path);
    out_path.replace(0, fs_local.length(), path_prefix);
    return out_path;
}
