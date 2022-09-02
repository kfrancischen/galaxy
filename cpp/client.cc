#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <google/protobuf/util/json_util.h>
#include <google/protobuf/message.h>

#include "cpp/client.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/core/galaxy_fs.h"
#include "cpp/util/galaxy_util.h"
#include "cpp/internal/galaxy_client_internal.h"
#include "cpp/internal/galaxy_const.h"
#include "absl/flags/flag.h"
#include "absl/container/flat_hash_map.h"
#include "glog/logging.h"

#include "include/rapidjson/document.h"
#include "include/rapidjson/prettywriter.h"
#include "include/rapidjson/stringbuffer.h"

using galaxy_schema::Owner;
using galaxy_schema::FileSystemStatus;
using galaxy_schema::FileSystemUsage;
using galaxy_schema::Credential;
using galaxy_schema::Attribute;
using galaxy_schema::WriteMode;

using galaxy_schema::SingleRequestCellConfigs;
using galaxy_schema::FileAnalyzerResult;

using galaxy_schema::CopyRequest;
using galaxy_schema::CopyResponse;
using galaxy_schema::CreateDirRequest;
using galaxy_schema::CreateDirResponse;
using galaxy_schema::CreateFileRequest;
using galaxy_schema::CreateFileResponse;
using galaxy_schema::CrossCellRequest;
using galaxy_schema::CrossCellResponse;
using galaxy_schema::DirOrDieRequest;
using galaxy_schema::DirOrDieResponse;
using galaxy_schema::FileOrDieRequest;
using galaxy_schema::FileOrDieResponse;
using galaxy_schema::GetAttrRequest;
using galaxy_schema::GetAttrResponse;
using galaxy_schema::ListDirsInDirRequest;
using galaxy_schema::ListDirsInDirResponse;
using galaxy_schema::ListFilesInDirRequest;
using galaxy_schema::ListFilesInDirResponse;
using galaxy_schema::ListAllInDirRecursiveRequest;
using galaxy_schema::ListAllInDirRecursiveResponse;
using galaxy_schema::ReadRequest;
using galaxy_schema::ReadResponse;
using galaxy_schema::ReadMultipleRequest;
using galaxy_schema::ReadMultipleResponse;
using galaxy_schema::RenameFileRequest;
using galaxy_schema::RenameFileResponse;
using galaxy_schema::RmDirRecursiveRequest;
using galaxy_schema::RmDirRecursiveResponse;
using galaxy_schema::RmDirRequest;
using galaxy_schema::RmDirResponse;
using galaxy_schema::RmFileRequest;
using galaxy_schema::RmFileResponse;
using galaxy_schema::WriteRequest;
using galaxy_schema::WriteResponse;
using galaxy_schema::WriteMultipleRequest;
using galaxy_schema::WriteMultipleResponse;
using galaxy_schema::HealthCheckRequest;
using galaxy_schema::HealthCheckResponse;

using galaxy::GalaxyClientInternal;
using galaxy::GalaxyFs;
using google::protobuf::Message;

GalaxyClientInternal GetChannelClient(const SingleRequestCellConfigs& config) {
    FLAGS_colorlogtostderr = true;
    FLAGS_log_dir = config.from_cell_config().fs_log_dir();
    google::EnableLogCleaner(config.from_cell_config().fs_log_ttl());
    grpc::ChannelArguments ch_args;
    ch_args.SetMaxReceiveMessageSize(-1);
    GalaxyClientInternal client(grpc::CreateCustomChannel(
        config.to_cell_config().fs_ip() + ":" + std::to_string(config.to_cell_config().fs_port()), grpc::InsecureChannelCredentials(), ch_args));
    return client;
}

std::string StatbufToString(const struct stat& statbuf) {
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    rapidjson::Value owner(rapidjson::kObjectType);
    owner.AddMember("uid", statbuf.st_uid, allocator);
    owner.AddMember("gid", statbuf.st_uid, allocator);
    doc.AddMember("owner", owner, allocator);
    doc.AddMember("dev", statbuf.st_dev, allocator);
    doc.AddMember("ino", statbuf.st_ino, allocator);
    doc.AddMember("mode", statbuf.st_mode, allocator);
    doc.AddMember("nlink", statbuf.st_nlink, allocator);
    doc.AddMember("rdev", statbuf.st_rdev, allocator);
    doc.AddMember("size", statbuf.st_size, allocator);
    doc.AddMember("blksize", statbuf.st_blksize, allocator);
    doc.AddMember("blocks", statbuf.st_blocks, allocator);
    doc.AddMember("atime", statbuf.st_atime, allocator);
    doc.AddMember("mtime", statbuf.st_mtime, allocator);
    doc.AddMember("ctime", statbuf.st_ctime, allocator);

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    doc.Accept(writer);
    return sb.GetString();
}

std::string ProtoMessageToString(const Message& message) {
    std::string output_str;
    google::protobuf::util::JsonPrintOptions option;
    option.add_whitespace = true;
    google::protobuf::util::MessageToJsonString(message, &output_str, option);
    return output_str;
}

void galaxy::client::impl::RCreateDirIfNotExist(const FileAnalyzerResult& result, const int mode) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        CreateDirRequest request;
        request.set_name(result.path());
        request.set_mode(mode);
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_from_cell(result.configs().from_cell_config().cell());
        CreateDirResponse response = client.CreateDirIfNotExist(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call CreateDirIfNotExist.";
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::RCopyFile(const FileAnalyzerResult& from_result, const FileAnalyzerResult& to_result) {
    try {
        GalaxyClientInternal client = GetChannelClient(to_result.configs());
        if (!from_result.is_remote()) {
            // Copy a local file to galaxy server
            CopyRequest request;
            request.mutable_cred()->set_password(to_result.configs().to_cell_config().fs_password());
            request.set_from_name(from_result.path());
            request.set_to_name(to_result.path());
            request.set_from_cell(from_result.configs().from_cell_config().cell());
            CopyResponse response = client.CopyFile(request);
            FileSystemStatus status = response.status();
            if (status.return_code() != 1) {
                throw "Fail to call CopyFile.";
            }
        } else {
            // Copy a remote file to galaxy server
            std::string to_galaxy_path = galaxy::util::ConvertToCellPath(to_result.path(), to_result.configs().to_cell_config());
            std::string prefix = galaxy::util::GetGalaxyFsPrefixPath(to_result.configs().to_cell_config().cell());
            if (to_galaxy_path.find(prefix) == std::string::npos) {
                throw "The to_path is not in galaxy.";
            }
            std::string from_galaxy_path = galaxy::util::ConvertToCellPath(from_result.path(), from_result.configs().to_cell_config());
            CrossCellRequest request;
            request.set_request_type("CopyFile");
            CopyRequest copy_request;
            copy_request.mutable_cred()->set_password(from_result.configs().to_cell_config().fs_password());
            copy_request.set_from_name(from_galaxy_path);
            copy_request.set_to_name(to_galaxy_path);
            copy_request.set_from_cell(from_result.configs().from_cell_config().cell());
            *request.mutable_cred() = copy_request.cred();
            request.mutable_request()->PackFrom(copy_request);
            CrossCellResponse response = client.CrossCellCall(request);
            CopyResponse copy_response;
            response.response().UnpackTo(&copy_response);
            FileSystemStatus status = copy_response.status();
            if (status.return_code() != 1) {
                throw "Fail to call CopyFile.";
            }
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::RMoveFile(const FileAnalyzerResult& from_result, const FileAnalyzerResult& to_result) {
    try {
        GalaxyClientInternal client = GetChannelClient(to_result.configs());
        if (!from_result.is_remote()) {
            // Copy a local file to galaxy server
            CopyRequest request;
            request.mutable_cred()->set_password(to_result.configs().to_cell_config().fs_password());
            request.set_from_name(from_result.path());
            request.set_to_name(to_result.path());
            request.set_from_cell(from_result.configs().from_cell_config().cell());
            CopyResponse response = client.CopyFile(request);
            FileSystemStatus status = response.status();
            if (status.return_code() != 1) {
                throw "Fail to call MoveFile.";
            } else {
                galaxy::client::RmFile(from_result.path());
            }
        } else {
            // Copy a remote file to galaxy server
            std::string to_galaxy_path = galaxy::util::ConvertToCellPath(to_result.path(), to_result.configs().to_cell_config());
            std::string prefix = galaxy::util::GetGalaxyFsPrefixPath(to_result.configs().to_cell_config().cell());
            if (to_galaxy_path.find(prefix) == std::string::npos) {
                throw "The to_path is not in galaxy.";
            }
            std::string from_galaxy_path = galaxy::util::ConvertToCellPath(from_result.path(), from_result.configs().to_cell_config());
            CrossCellRequest request;
            request.set_request_type("MoveFile");
            CopyRequest copy_request;
            copy_request.mutable_cred()->set_password(from_result.configs().to_cell_config().fs_password());
            copy_request.set_from_name(from_galaxy_path);
            copy_request.set_to_name(to_galaxy_path);
            copy_request.set_from_cell(from_result.configs().from_cell_config().cell());
            request.mutable_request()->PackFrom(copy_request);
            CrossCellResponse response = client.CrossCellCall(request);
            CopyResponse copy_response;
            response.response().UnpackTo(&copy_response);
            FileSystemStatus status = copy_response.status();
            if (status.return_code() != 1) {
                throw "Fail to call MoveFile.";
            }
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::RDirOrDie(const FileAnalyzerResult& result) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        DirOrDieRequest request;
        request.set_name(result.path());
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_from_cell(result.configs().from_cell_config().cell());
        DirOrDieResponse response = client.DirOrDie(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call DirOrDie.";
        }
        return galaxy::util::ConvertToCellPath(response.name(), result.configs().to_cell_config());
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::impl::RRmDir(const FileAnalyzerResult& result, bool include_hidden) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        RmDirRequest request;
        request.set_name(result.path());
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_include_hidden(include_hidden);
        request.set_from_cell(result.configs().from_cell_config().cell());
        RmDirResponse response = client.RmDir(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call RmDir.";
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::RRmDirRecursive(const FileAnalyzerResult& result, bool include_hidden) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        RmDirRecursiveRequest request;
        request.set_name(result.path());
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_include_hidden(include_hidden);
        request.set_from_cell(result.configs().from_cell_config().cell());
        RmDirRecursiveResponse response = client.RmDirRecursive(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call RmDirRecursive.";
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::map<std::string, std::string> galaxy::client::impl::RListDirsInDir(const FileAnalyzerResult& result) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        ListDirsInDirRequest request;
        request.set_name(result.path());
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_from_cell(result.configs().from_cell_config().cell());
        ListDirsInDirResponse response = client.ListDirsInDir(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call ListDirsInDir.";
        }
        std::map<std::string, std::string> dirs;
        for (const auto &sub_dir : response.sub_dirs()) {
            dirs.insert({galaxy::util::ConvertToCellPath(sub_dir.first, result.configs().to_cell_config()),
                         ProtoMessageToString(sub_dir.second)});
        }
        return dirs;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::RListFilesInDir(const FileAnalyzerResult& result, bool include_hidden) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        ListFilesInDirRequest request;
        request.set_name(result.path());
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_include_hidden(include_hidden);
        request.set_from_cell(result.configs().from_cell_config().cell());
        ListFilesInDirResponse response = client.ListFilesInDir(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call ListFilesInDir.";
        }
        std::map<std::string, std::string> files;
        for (const auto& sub_file : response.sub_files()) {
            files.insert({galaxy::util::ConvertToCellPath(sub_file.first, result.configs().to_cell_config()),
                          ProtoMessageToString(sub_file.second)});
        }
        return files;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::RListDirsInDirRecursive(const FileAnalyzerResult& result) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        ListAllInDirRecursiveRequest request;
        request.set_name(result.path());
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_from_cell(result.configs().from_cell_config().cell());
        ListAllInDirRecursiveResponse response = client.ListAllInDirRecursive(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call RListDirsInDirRecursive.";
        }
        std::map<std::string, std::string> dirs;
        for (const auto& sub_dir : response.sub_dirs()) {
            dirs.insert({galaxy::util::ConvertToCellPath(sub_dir.first, result.configs().to_cell_config()),
                         ProtoMessageToString(sub_dir.second)});
        }
        return dirs;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::RListFilesInDirRecursive(const FileAnalyzerResult& result, bool include_hidden) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        ListAllInDirRecursiveRequest request;
        request.set_name(result.path());
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_include_hidden(include_hidden);
        request.set_from_cell(result.configs().from_cell_config().cell());
        ListAllInDirRecursiveResponse response = client.ListAllInDirRecursive(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call RListFilesInDirRecursive.";
        }
        std::map<std::string, std::string> files;
        for (const auto& sub_file : response.sub_files()) {
            files.insert({galaxy::util::ConvertToCellPath(sub_file.first, result.configs().to_cell_config()),
                          ProtoMessageToString(sub_file.second)});
        }
        return files;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

void galaxy::client::impl::RCreateFileIfNotExist(const FileAnalyzerResult& result, const int mode) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        CreateFileRequest request;
        request.set_name(result.path());
        request.set_mode(mode);
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_from_cell(result.configs().from_cell_config().cell());
        CreateFileResponse response = client.CreateFileIfNotExist(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call CreateFileIfNotExist.";
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::RFileOrDie(const FileAnalyzerResult& result) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        FileOrDieRequest request;
        request.set_name(result.path());
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_from_cell(result.configs().from_cell_config().cell());
        FileOrDieResponse response = client.FileOrDie(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call FileOrDie.";
        }
        return galaxy::util::ConvertToCellPath(response.name(), result.configs().to_cell_config());
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::impl::RRmFile(const FileAnalyzerResult& result, bool is_hidden) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        RmFileRequest request;
        request.set_name(result.path());
        request.set_is_hidden(is_hidden);
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_from_cell(result.configs().from_cell_config().cell());
        RmFileResponse response = client.RmFile(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call RmFile.";
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::RRenameFile(const FileAnalyzerResult& old_result, const FileAnalyzerResult& new_result) {
    GalaxyClientInternal client = GetChannelClient(old_result.configs());
    try {
        RenameFileRequest request;
        request.set_old_name(old_result.path());
        request.set_new_name(new_result.path());
        request.mutable_cred()->set_password(old_result.configs().to_cell_config().fs_password());
        request.set_from_cell(old_result.configs().from_cell_config().cell());
        RenameFileResponse response = client.RenameFile(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call RenameFile.";
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::RRead(const FileAnalyzerResult& result) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        ReadRequest request;
        request.set_name(result.path());
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_from_cell(result.configs().from_cell_config().cell());
        ReadResponse response = client.Read(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call Read.";
        }
        return response.data();
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

std::map<std::string, std::string> galaxy::client::impl::RReadMultiple(const std::vector<FileAnalyzerResult>& results) {
    GalaxyClientInternal client = GetChannelClient(results.at(0).configs());
    ReadMultipleRequest request;
    std::map<std::string, std::string> data_map;
    for (const auto& result : results) {
        request.add_names(result.path());
    }
    request.mutable_cred()->set_password(results.at(0).configs().to_cell_config().fs_password());
    request.set_from_cell(results.at(0).configs().from_cell_config().cell());
    ReadMultipleResponse response = client.ReadMultiple(request);
    for (const auto& pair : response.data()) {
        std::string path = galaxy::util::ConvertToCellPath(pair.first, results.at(0).configs().to_cell_config());
        if (pair.second.status().return_code() != 1) {
            LOG(ERROR) << "Failed to read data for file " << path;
        } else {
            data_map.insert({path, pair.second.data()});
        }
    }
    return data_map;
}

void galaxy::client::impl::RWrite(const FileAnalyzerResult& result, const std::string& data, const std::string& mode) {
    CHECK(mode == "a" || mode == "w") << "Mode has to be either a or w";
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        WriteRequest request;
        request.set_name(result.path());
        request.set_data(data);
        if (mode == "a") {
            request.set_mode(WriteMode::APPEND);
        } else {
            request.set_mode(WriteMode::OVERWRITE);
        }
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_from_cell(result.configs().from_cell_config().cell());
        WriteResponse response = client.Write(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call Write.";
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::RWriteMultiple(const std::vector<std::pair<galaxy_schema::FileAnalyzerResult, std::string>>& path_data_map, const std::string& mode) {
    GalaxyClientInternal client = GetChannelClient(path_data_map.begin()->first.configs());
    CHECK(mode == "a" || mode == "w");
    try {
        WriteMultipleRequest request;
        if (mode == "a") {
            request.set_mode(WriteMode::APPEND);
        } else {
            request.set_mode(WriteMode::OVERWRITE);
        }
        request.mutable_cred()->set_password(path_data_map.begin()->first.configs().to_cell_config().fs_password());
        request.set_from_cell(path_data_map.begin()->first.configs().from_cell_config().cell());

        for (const auto& val : path_data_map) {
            (*request.mutable_data())[val.first.path()] = val.second;
        }
        WriteMultipleResponse response = client.WriteMultiple(request);
        for (const auto& pair : response.data()) {
            std::string path = galaxy::util::ConvertToCellPath(pair.first, path_data_map.begin()->first.configs().to_cell_config());
            if (pair.second.status().return_code() != 1) {
                LOG(ERROR) << "Failed to write data for file " << path;
            }
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::RGetAttr(const FileAnalyzerResult& result) {
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        GetAttrRequest request;
        request.set_name(result.path());
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_from_cell(result.configs().from_cell_config().cell());
        GetAttrResponse response = client.GetAttr(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call GetAttr.";
        }
        return ProtoMessageToString(response.attr());
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

std::string galaxy::client::impl::RCheckHealth(const std::string& cell) {
    std::string path = galaxy::util::GetGalaxyFsPrefixPath(cell);
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    GalaxyClientInternal client = GetChannelClient(result.configs());
    try {
        HealthCheckRequest request;
        request.mutable_cred()->set_password(result.configs().to_cell_config().fs_password());
        request.set_from_cell(result.configs().from_cell_config().cell());
        HealthCheckResponse response = client.CheckHealth(request);
        if (!response.healthy()) {
            throw "cell " + cell +" is unhealthy.";
        }
        return ProtoMessageToString(response);
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::impl::LCreateDirIfNotExist(const FileAnalyzerResult& result, const int mode) {
    try {
        GalaxyFs fs("");
        auto status = fs.CreateDirIfNotExist(result.path(), mode);
        if (!status.ok()) {
            throw "CreateDirIfNotExist failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::LCopyFile(const FileAnalyzerResult& from_result, const FileAnalyzerResult& to_result) {
    try {
        GalaxyFs fs("");
        auto status = fs.CopyFile(from_result.path(), to_result.path());
        if (!status.ok()) {
            throw "CopyFile failed with error " + status.ToString() + '.';
        }
    } catch (std::string errorMsg) {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::LMoveFile(const FileAnalyzerResult& from_result, const FileAnalyzerResult& to_result) {
    try {
        GalaxyFs fs("");
        auto status = fs.MoveFile(from_result.path(), to_result.path());
        if (!status.ok()) {
            throw "MoveFile failed with error " + status.ToString() + '.';
        }
    } catch (std::string errorMsg) {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::LDirOrDie(const FileAnalyzerResult& result) {
    try {
        GalaxyFs fs("");
        std::string out_path;
        auto status = fs.DieDirIfNotExist(result.path(), out_path);
        if (!status.ok()) {
            throw "DirOrDie failed with error " + status.ToString() + '.';
        }
        return galaxy::util::ConvertToCellPath(out_path, result.configs().from_cell_config());
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::impl::LRmDir(const FileAnalyzerResult& result, bool include_hidden) {
    try {
        GalaxyFs fs("");
        auto status = fs.RmDir(result.path(), include_hidden);
        if (!status.ok()) {
            throw "RmDir failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::LRmDirRecursive(const FileAnalyzerResult& result, bool include_hidden) {
    try {
        GalaxyFs fs("");
        auto status = fs.RmDirRecursive(result.path(), include_hidden);
        if (!status.ok()) {
            throw "RmDirRecursive failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::map<std::string, std::string> galaxy::client::impl::LListDirsInDir(const FileAnalyzerResult& result) {
    try {
        absl::flat_hash_map<std::string, struct stat> sub_dirs;
        GalaxyFs fs("");
        auto status = fs.ListDirsInDir(result.path(), sub_dirs);
        if (!status.ok()) {
            throw "ListDirsInDir failed with error " + status.ToString() + '.';
        }
        std::map<std::string, std::string> dirs;
        for (const auto& sub_dir : sub_dirs) {
            dirs.insert({galaxy::util::ConvertToCellPath(sub_dir.first, result.configs().from_cell_config()), StatbufToString(sub_dir.second)});
        }
        return dirs;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::LListFilesInDir(const FileAnalyzerResult& result, bool include_hidden) {
    try {
        absl::flat_hash_map<std::string, struct stat> sub_files;
        GalaxyFs fs("");
        auto status = fs.ListFilesInDir(result.path(), sub_files, include_hidden);
        if (!status.ok()) {
            throw "ListFilesInDir failed with error " + status.ToString() + '.';
        }
        std::map<std::string, std::string> files;
        for (const auto& sub_file : sub_files) {
            files.insert({galaxy::util::ConvertToCellPath(sub_file.first, result.configs().from_cell_config()), StatbufToString(sub_file.second)});
        }
        return files;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::LListDirsInDirRecursive(const FileAnalyzerResult& result) {
    try {
        absl::flat_hash_map<std::string, struct stat> sub_files;
        absl::flat_hash_map<std::string, struct stat> sub_dirs;
        GalaxyFs fs("");
        auto status = fs.ListAllInDirRecursive(result.path(), sub_dirs, sub_files);
        if (!status.ok()) {
            throw "ListDirsInDirRecursive failed with error " + status.ToString() + '.';
        }
        std::map<std::string, std::string> dirs;
        for (const auto& sub_dir : sub_dirs) {
            dirs.insert({galaxy::util::ConvertToCellPath(sub_dir.first, result.configs().from_cell_config()), StatbufToString(sub_dir.second)});
        }
        return dirs;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::LListFilesInDirRecursive(const FileAnalyzerResult& result, bool include_hidden) {
    try {
        absl::flat_hash_map<std::string, struct stat> sub_files;
        absl::flat_hash_map<std::string, struct stat> sub_dirs;
        GalaxyFs fs("");
        auto status = fs.ListAllInDirRecursive(result.path(), sub_dirs, sub_files, include_hidden);
        if (!status.ok()) {
            throw "ListFilesInDirRecursive failed with error " + status.ToString() + '.';
        }
        std::map<std::string, std::string> files;
        for (const auto& sub_file : sub_files) {
            files.insert({galaxy::util::ConvertToCellPath(sub_file.first, result.configs().from_cell_config()), StatbufToString(sub_file.second)});
        }
        return files;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}


void galaxy::client::impl::LCreateFileIfNotExist(const FileAnalyzerResult& result, const int mode) {
    try {
        GalaxyFs fs("");
        auto status = fs.CreateFileIfNotExist(result.path(), mode);
        if (!status.ok()) {
            throw "CreateFileIfNotExist failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::LFileOrDie(const FileAnalyzerResult& result) {
    try {
        GalaxyFs fs("");
        std::string out_path;
        auto status = fs.DieFileIfNotExist(result.path(), out_path);
        if (!status.ok()) {
            throw "FileOrDie failed with error " + status.ToString() + '.';
        }
        return galaxy::util::ConvertToCellPath(out_path, result.configs().from_cell_config());
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::impl::LRmFile(const FileAnalyzerResult& result, bool is_hidden) {
    try {
        GalaxyFs fs("");
        auto status = fs.RmFile(result.path(), !is_hidden);
        if (!status.ok()) {
            throw "RmFile failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::LRenameFile(const FileAnalyzerResult& old_result, const FileAnalyzerResult& new_result) {
    try {
        GalaxyFs fs("");
        auto status = fs.RenameFile(old_result.path(), new_result.path());
        if (!status.ok()) {
            throw "RenameFile failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::LRead(const FileAnalyzerResult& result) {
    try {
        GalaxyFs fs("");
        std::string data;
        auto status = fs.Read(result.path(), data);
        if (!status.ok()) {
            throw "Read failed with error " + status.ToString() + '.';
        }
        return data;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

std::map<std::string, std::string> galaxy::client::impl::LReadMultiple(const std::vector<FileAnalyzerResult>& results) {
    GalaxyFs fs("");
    std::map<std::string, std::string> data_map;
    for (const auto& result : results) {
        std::string data;
        auto status = fs.Read(result.path(), data);
        if (!status.ok()) {
            LOG(ERROR) << "Read " << result.path() <<" failed with error " << status.ToString();
            data_map.insert({galaxy::util::ConvertToCellPath(result.path(), result.configs().from_cell_config()), ""});
        } else {
            data_map.insert({galaxy::util::ConvertToCellPath(result.path(), result.configs().from_cell_config()), data});
        }
    }
    return data_map;
}

void galaxy::client::impl::LWrite(const FileAnalyzerResult& result, const std::string& data, const std::string& mode) {
    try {
        GalaxyFs fs("");
        auto status = fs.Write(result.path(), data, mode);
        if (!status.ok()) {
            throw "Write failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }

}

void galaxy::client::impl::LWriteMultiple(const std::vector<std::pair<galaxy_schema::FileAnalyzerResult, std::string>>& path_data_map, const std::string& mode) {
    GalaxyFs fs("");
    for (const auto& val : path_data_map) {
        auto status = fs.Write(val.first.path(), val.second, mode);
        if (!status.ok()) {
            LOG(ERROR) << "Write " << val.first.path() <<" failed with error " << status.ToString();
        }
    }
}

std::string galaxy::client::impl::LGetAttr(const FileAnalyzerResult& result) {
    try {
        GalaxyFs fs("");
        struct stat statbuf;
        auto status = fs.GetAttr(result.path(), &statbuf);
        if (!status.ok()) {
            throw "GetAttr failed with error " + status.ToString() + '.';
        }
        return StatbufToString(statbuf);
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}


// actual functions calls
void galaxy::client::CreateDirIfNotExist(const std::string& path, const int mode) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RCreateDirIfNotExist(result, mode);
    } else {
        VLOG(1) << "Using local mode";
        galaxy::client::impl::LCreateDirIfNotExist(result, mode);
    }
}

std::string galaxy::client::DirOrDie(const std::string& path) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RDirOrDie(result);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LDirOrDie(result);
    }
}

void galaxy::client::RmDir(const std::string& path, bool include_hidden) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RRmDir(result, include_hidden);
    } else {
        VLOG(1) << "Using local mode";
        galaxy::client::impl::LRmDir(result, include_hidden);
    }
}

void galaxy::client::RmDirRecursive(const std::string& path, bool include_hidden) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RRmDirRecursive(result, include_hidden);
    } else {
        VLOG(1) << "Using local mode";
        galaxy::client::impl::LRmDirRecursive(result, include_hidden);
    }
}

std::map<std::string, std::string> galaxy::client::ListDirsInDir(const std::string& path) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListDirsInDir(result);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LListDirsInDir(result);
    }
}

std::map<std::string, std::string> galaxy::client::ListFilesInDir(const std::string& path, bool include_hidden) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListFilesInDir(result);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LListFilesInDir(result);
    }
}

std::map<std::string, std::string> galaxy::client::ListDirsInDirRecursive(const std::string& path) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListDirsInDirRecursive(result);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LListDirsInDirRecursive(result);
    }
}

std::map<std::string, std::string> galaxy::client::ListFilesInDirRecursive(const std::string& path, bool include_hidden) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListFilesInDirRecursive(result, include_hidden);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LListFilesInDirRecursive(result, include_hidden);
    }
}

void galaxy::client::CreateFileIfNotExist(const std::string& path, const int mode) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RCreateFileIfNotExist(result, mode);
    } else {
        VLOG(1) << "Using local mode";
        galaxy::client::impl::LCreateFileIfNotExist(result, mode);
    }
}

void galaxy::client::CopyFile(const std::string& from_path, const std::string& to_path) {
    FileAnalyzerResult from_result = galaxy::util::InitClient(from_path);
    FileAnalyzerResult to_result = galaxy::util::InitClient(to_path);
    // If the path is a local path.
    if (!from_result.is_remote() && !to_result.is_remote()) {
        VLOG(1) << "Both path are using local mode";
        galaxy::client::impl::LCopyFile(from_result, to_result);
    } else {
        VLOG(2) << "Using remote mode for at least one of the paths";
        galaxy::client::impl::RCopyFile(from_result, to_result);
    }
}

void galaxy::client::MoveFile(const std::string& from_path, const std::string& to_path) {
    FileAnalyzerResult from_result = galaxy::util::InitClient(from_path);
    FileAnalyzerResult to_result = galaxy::util::InitClient(to_path);
    // If the path is a local path.
    if (!from_result.is_remote() && !to_result.is_remote()) {
        VLOG(1) << "Both path are using local mode";
        galaxy::client::impl::LMoveFile(from_result, to_result);
    } else {
        VLOG(2) << "Using remote mode for at least one of the paths";
        galaxy::client::impl::RMoveFile(from_result, to_result);
    }
}

std::string galaxy::client::FileOrDie(const std::string& path) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RFileOrDie(result);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LFileOrDie(result);
    }
}

void galaxy::client::RmFile(const std::string& path, bool is_hidden) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RRmFile(result, is_hidden);
    } else {
        VLOG(1) << "Using local mode";
        galaxy::client::impl::LRmFile(result, is_hidden);
    }
}

void galaxy::client::RenameFile(const std::string& old_path, const std::string& new_path) {
    FileAnalyzerResult old_result = galaxy::util::InitClient(old_path);
    FileAnalyzerResult new_result = galaxy::util::InitClient(new_path);
    CHECK_EQ(old_result.is_remote(), new_result.is_remote()) << "Both paths need to be either remote or local.";
    if (old_result.is_remote()) {
        VLOG(2) << "Using remote mode";
        CHECK_EQ(old_result.configs().to_cell_config().cell(), new_result.configs().to_cell_config().cell()) << "Files are in different cells.";
        galaxy::client::impl::RRenameFile(old_result, new_result);
    } else {
        VLOG(1) << "Using local mode";
        galaxy::client::impl::LRenameFile(old_result, new_result);
    }

}

std::string galaxy::client::Read(const std::string& path) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RRead(result);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LRead(result);
    }
}

std::map<std::string, std::string> galaxy::client::ReadMultiple(const std::vector<std::string>& paths) {
    std::vector<FileAnalyzerResult> local_results, remote_results;
    std::string cell = "";
    for (const auto& path : paths) {
        FileAnalyzerResult result = galaxy::util::InitClient(path);
        if (result.is_remote()) {
            if (cell.empty()) {
                cell = result.configs().to_cell_config().cell();
            } else {
                CHECK_EQ(cell, result.configs().to_cell_config().cell()) << "Only support multiple read to the same cell.";
            }
            remote_results.push_back(result);
        } else {
            local_results.push_back(result);
        }
    }

    std::map<std::string, std::string> data_map;
    if (!remote_results.empty()) {
        std::map<std::string, std::string> remote_result = galaxy::client::impl::RReadMultiple(remote_results);
        data_map.insert(remote_result.begin(), remote_result.end());
    }
    if (!local_results.empty()) {
        std::map<std::string, std::string> local_result = galaxy::client::impl::LReadMultiple(local_results);
        data_map.insert(local_result.begin(), local_result.end());
    }
    return data_map;
}


void galaxy::client::Write(const std::string& path, const std::string& data, const std::string& mode) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RWrite(result, data, mode);
    } else {
        VLOG(1) << "Using local mode";
        galaxy::client::impl::LWrite(result, data, mode);
    }
}

void galaxy::client::WriteMultiple(const std::map<std::string, std::string>& path_data_map, const std::string& mode) {
    std::vector<std::pair<FileAnalyzerResult, std::string>> local_data, remote_data;
    std::string cell = "";
    for (const auto& val : path_data_map) {
        FileAnalyzerResult result = galaxy::util::InitClient(val.first);
        if (result.is_remote()) {
            if (cell.empty()) {
                cell = result.configs().to_cell_config().cell();
            } else {
                CHECK_EQ(cell, result.configs().to_cell_config().cell()) << "Only support multiple write to the same cell.";
            }
            remote_data.push_back(std::make_pair(result, val.second));
        } else {
            local_data.push_back(std::make_pair(result, val.second));
        }
    }
    if (!remote_data.empty()) {
        galaxy::client::impl::RWriteMultiple(remote_data, mode);
    }
    if (!local_data.empty()) {
        galaxy::client::impl::LWriteMultiple(local_data, mode);
    }
}

std::string galaxy::client::GetAttr(const std::string& path) {
    FileAnalyzerResult result = galaxy::util::InitClient(path);
    // If the path is a local path.
    if(result.is_remote()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RGetAttr(result);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LGetAttr(result);
    }
}


std::vector<std::string> galaxy::client::ListCells() {
    return galaxy::util::GetAllCells();
}

std::string galaxy::client::CheckHealth(const std::string& cell) {
    if (!cell.empty()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RCheckHealth(cell);
    } else {
        VLOG(1) << "Local mode doest not support health check.";
        return "";
    }
}
