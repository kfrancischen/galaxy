#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "cpp/util/galaxy_util.h"
#include "cpp/core/galaxy_flag.h"
#include "include/rapidjson/istreamwrapper.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/prettywriter.h"
#include "include/rapidjson/stringbuffer.h"
#include "absl/flags/flag.h"
#include "absl/status/status.h"
#include "absl/strings/str_split.h"
#include "absl/strings/str_join.h"
#include "glog/logging.h"

absl::StatusOr<std::string> galaxy::util::ParseGlobalConfig(bool is_server) {
    std::string config_path = absl::GetFlag(FLAGS_fs_global_config);
    std::string cell = absl::GetFlag(FLAGS_fs_cell);
    std::ifstream infile;
    infile.open(config_path);
    if (infile) {
        rapidjson::IStreamWrapper isw(infile);
        rapidjson::Document doc;
        if (doc.ParseStream(isw).HasParseError()) {
            return absl::InternalError("Configuration json cannot be parsed.");
        }
        if (!doc.HasMember(cell.c_str())) {
            return absl::InternalError("Cell configuration cannot be found.");
        }
        const rapidjson::Value& cell_config = doc[cell.c_str()];

        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
        cell_config.Accept(writer);
        if (!cell_config.HasMember("fs_root") || !cell_config.HasMember("fs_ip") || !cell_config.HasMember("fs_port") || !cell_config.HasMember("fs_password")) {
            return absl::FailedPreconditionError("Imcomplete configuration file. The file should at least contain fs_root, fs_ip, fs_port and fs_password.");
        }
        absl::SetFlag(&FLAGS_fs_root, cell_config["fs_root"].GetString());
        absl::SetFlag(&FLAGS_fs_password, cell_config["fs_password"].GetString());
        std::string fs_ip = cell_config["fs_ip"].GetString();
        int fs_port = cell_config["fs_port"].GetInt();
        if (is_server) {
            absl::SetFlag(&FLAGS_fs_address, "0.0.0.0:" + std::to_string(fs_port));
        } else {
            absl::SetFlag(&FLAGS_fs_address, fs_ip + ":" + std::to_string(fs_port));
        }

        if (cell_config.HasMember("fs_log_ttl") && is_server) {
            absl::SetFlag(&FLAGS_fs_log_ttl, cell_config["fs_log_ttl"].GetInt());
        }
        if (cell_config.HasMember("fs_log_dir") && is_server) {
            absl::SetFlag(&FLAGS_fs_log_dir, cell_config["fs_log_dir"].GetString());
        }
        if (cell_config.HasMember("fs_verbose_level") && is_server) {
            absl::SetFlag(&FLAGS_fs_verbose_level, cell_config["fs_verbose_level"].GetInt());
        }
        if (cell_config.HasMember("fs_alsologtostderr") && is_server) {
            absl::SetFlag(&FLAGS_fs_alsologtostderr, cell_config["fs_alsologtostderr"].GetBool());
        }
        return "Getting cell config for cell [" + cell + "]:\n" + sb.GetString();
    } else {
        return absl::NotFoundError(config_path + " does not exist.");
    }
}

absl::StatusOr<std::pair<std::string, std::string>> galaxy::util::GetCellAndPathFromPath(const std::string& path) {
    if (path.empty() || path[0] != kSeperator) {
        return absl::InvalidArgumentError("Input path is invalid.");
    }
    std::string path_copy(path);
    path_copy.erase(path_copy.begin());
    std::vector<std::string> v = absl::StrSplit(path_copy, kSeperator);
    std::string cell_suffix(kCellSuffix);
    if (v.size() < 1 || v[0].find(cell_suffix) == std::string::npos) {
        return absl::InvalidArgumentError("Input path is invalid.");
    }
    std::string cell = v[0];
    for (size_t i = 0; i< cell_suffix.length(); i++) {
        cell.pop_back();
    }
    v.erase(v.begin());
    std::string seprator(1, kSeperator);
    std::string file_path = absl::StrJoin(v, seprator);
    return std::make_pair(cell, file_path);
}
