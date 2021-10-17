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

using galaxy_schema::CreateDirRequest;
using galaxy_schema::CreateDirResponse;
using galaxy_schema::CreateFileRequest;
using galaxy_schema::CreateFileResponse;
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

GalaxyClientInternal GetChannelClient(const std::string& cell) {
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false, cell);
    FLAGS_colorlogtostderr = true;
    FLAGS_log_dir = absl::GetFlag(FLAGS_fs_log_dir);
    google::EnableLogCleaner(absl::GetFlag(FLAGS_fs_log_ttl));
    CHECK(result.ok()) << "Fail to parse the global config.";
    grpc::ChannelArguments ch_args;
    ch_args.SetMaxReceiveMessageSize(-1);
    GalaxyClientInternal client(grpc::CreateCustomChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials(), ch_args));
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

void galaxy::client::impl::RCreateDirIfNotExist(const std::string& path, const int mode) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        CreateDirRequest request;
        request.set_name(path);
        request.set_mode(mode);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
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

std::string galaxy::client::impl::RDirOrDie(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        DirOrDieRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        DirOrDieResponse response = client.DirOrDie(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call DirOrDie.";
        }
        return galaxy::util::MapToCellPath(response.name());
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::impl::RRmDir(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        RmDirRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
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

void galaxy::client::impl::RRmDirRecursive(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        RmDirRecursiveRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
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

std::map<std::string, std::string> galaxy::client::impl::RListDirsInDir(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        ListDirsInDirRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListDirsInDirResponse response = client.ListDirsInDir(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call ListDirsInDir.";
        }
        std::map<std::string, std::string> result;
        for (const auto& sub_dir : response.sub_dirs()) {
            result.insert({galaxy::util::MapToCellPath(sub_dir.first), ProtoMessageToString(sub_dir.second)});
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::RListFilesInDir(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        ListFilesInDirRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListFilesInDirResponse response = client.ListFilesInDir(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call ListFilesInDir.";
        }
        std::map<std::string, std::string> result;
        for (const auto& sub_file : response.sub_files()) {
            result.insert({galaxy::util::MapToCellPath(sub_file.first), ProtoMessageToString(sub_file.second)});
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::RListDirsInDirRecursive(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        ListAllInDirRecursiveRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListAllInDirRecursiveResponse response = client.ListAllInDirRecursive(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call RListDirsInDirRecursive.";
        }
        std::map<std::string, std::string> result;
        for (const auto& sub_dir : response.sub_dirs()) {
            result.insert({galaxy::util::MapToCellPath(sub_dir.first), ProtoMessageToString(sub_dir.second)});
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::RListFilesInDirRecursive(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        ListAllInDirRecursiveRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListAllInDirRecursiveResponse response = client.ListAllInDirRecursive(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call RListFilesInDirRecursive.";
        }
        std::map<std::string, std::string> result;
        for (const auto& sub_file : response.sub_files()) {
            result.insert({galaxy::util::MapToCellPath(sub_file.first), ProtoMessageToString(sub_file.second)});
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

void galaxy::client::impl::RCreateFileIfNotExist(const std::string& path, const int mode) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        CreateFileRequest request;
        request.set_name(path);
        request.set_mode(mode);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
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

std::string galaxy::client::impl::RFileOrDie(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        FileOrDieRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        FileOrDieResponse response = client.FileOrDie(request);
        FileSystemStatus status = response.status();
        if (status.return_code() != 1) {
            throw "Fail to call FileOrDie.";
        }
        return galaxy::util::MapToCellPath(response.name());
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::impl::RRmFile(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        RmFileRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
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

void galaxy::client::impl::RRenameFile(const std::string& old_path, const std::string& new_path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        RenameFileRequest request;
        request.set_old_name(old_path);
        request.set_new_name(new_path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
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

std::string galaxy::client::impl::RRead(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        ReadRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
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

std::map<std::string, std::string> galaxy::client::impl::RReadMultiple(const std::vector<std::string>& paths) {
    GalaxyClientInternal client = GetChannelClient("");
    ReadMultipleRequest request;
    std::map<std::string, std::string> result;
    *request.mutable_names() = {paths.begin(), paths.end()};
    request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
    ReadMultipleResponse response = client.ReadMultiple(request);
    for (const auto& pair : response.data()) {
        std::string path = galaxy::util::MapToCellPath(pair.first);
        if (pair.second.status().return_code() != 1) {
            LOG(ERROR) << "Failed to read data for file " << path;
        } else {
            result.insert({path, pair.second.data()});
        }
    }
    return result;
}

void galaxy::client::impl::RWrite(const std::string& path, const std::string& data, const std::string& mode) {
    GalaxyClientInternal client = GetChannelClient("");
    CHECK(mode == "a" || mode == "w");
    try {
        WriteRequest request;
        request.set_name(path);
        request.set_data(data);
        if (mode == "a") {
            request.set_mode(WriteMode::APPEND);
        } else {
            request.set_mode(WriteMode::OVERWRITE);
        }
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
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

void galaxy::client::impl::RWriteMultiple(const std::map<std::string, std::string>& path_data_map, const std::string& mode) {
    GalaxyClientInternal client = GetChannelClient("");
    CHECK(mode == "a" || mode == "w");
    try {
        WriteMultipleRequest request;
        if (mode == "a") {
            request.set_mode(WriteMode::APPEND);
        } else {
            request.set_mode(WriteMode::OVERWRITE);
        }
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        *request.mutable_data() = {path_data_map.begin(), path_data_map.end()};
        WriteMultipleResponse response = client.WriteMultiple(request);
        for (const auto& pair : response.data()) {
            std::string path = galaxy::util::MapToCellPath(pair.first);
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

std::string galaxy::client::impl::RGetAttr(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient("");
    try {
        GetAttrRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
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
    GalaxyClientInternal client = GetChannelClient(cell);
    try {
        HealthCheckRequest request;
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
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

void galaxy::client::impl::LCreateDirIfNotExist(const std::string& path, const int mode) {
    try {
        GalaxyFs fs("");
        auto status = fs.CreateDirIfNotExist(path, mode);
        if (!status.ok()) {
            throw "CreateDirIfNotExist failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::LDirOrDie(const std::string& path) {
    try {
        GalaxyFs fs("");
        std::string out_path;
        auto status = fs.DieDirIfNotExist(path, out_path);
        if (!status.ok()) {
            throw "DirOrDie failed with error " + status.ToString() + '.';
        }
        return out_path;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::impl::LRmDir(const std::string& path) {
    try {
        GalaxyFs fs("");
        auto status = fs.RmDir(path);
        if (!status.ok()) {
            throw "RmDir failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::LRmDirRecursive(const std::string& path) {
    try {
        GalaxyFs fs("");
        auto status = fs.RmDirRecursive(path);
        if (!status.ok()) {
            throw "RmDirRecursive failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::map<std::string, std::string> galaxy::client::impl::LListDirsInDir(const std::string& path) {
    try {
        absl::flat_hash_map<std::string, struct stat> sub_dirs;
        GalaxyFs fs("");
        auto status = fs.ListDirsInDir(path, sub_dirs);
        if (!status.ok()) {
            throw "ListDirsInDir failed with error " + status.ToString() + '.';
        }
        std::map<std::string, std::string> result;
        for (const auto& sub_dir : sub_dirs) {
            result.insert({sub_dir.first, StatbufToString(sub_dir.second)});
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::LListFilesInDir(const std::string& path) {
    try {
        absl::flat_hash_map<std::string, struct stat> sub_files;
        GalaxyFs fs("");
        auto status = fs.ListFilesInDir(path, sub_files);
        if (!status.ok()) {
            throw "ListFilesInDir failed with error " + status.ToString() + '.';
        }
        std::map<std::string, std::string> result;
        for (const auto& sub_file : sub_files) {
            result.insert({sub_file.first, StatbufToString(sub_file.second)});
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::LListDirsInDirRecursive(const std::string& path) {
    try {
        absl::flat_hash_map<std::string, struct stat> sub_files;
        absl::flat_hash_map<std::string, struct stat> sub_dirs;
        GalaxyFs fs("");
        auto status = fs.ListAllInDirRecursive(path, sub_dirs, sub_files);
        if (!status.ok()) {
            throw "ListDirsInDirRecursive failed with error " + status.ToString() + '.';
        }
        std::map<std::string, std::string> result;
        for (const auto& sub_dir : sub_dirs) {
            result.insert({sub_dir.first, StatbufToString(sub_dir.second)});
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}

std::map<std::string, std::string> galaxy::client::impl::LListFilesInDirRecursive(const std::string& path) {
    try {
        absl::flat_hash_map<std::string, struct stat> sub_files;
        absl::flat_hash_map<std::string, struct stat> sub_dirs;
        GalaxyFs fs("");
        auto status = fs.ListAllInDirRecursive(path, sub_dirs, sub_files);
        if (!status.ok()) {
            throw "ListFilesInDirRecursive failed with error " + status.ToString() + '.';
        }
        std::map<std::string, std::string> result;
        for (const auto& sub_file : sub_files) {
            result.insert({sub_file.first, StatbufToString(sub_file.second)});
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::map<std::string, std::string>();
    }
}


void galaxy::client::impl::LCreateFileIfNotExist(const std::string& path, const int mode) {
    try {
        GalaxyFs fs("");
        auto status = fs.CreateFileIfNotExist(path, mode);
        if (!status.ok()) {
            throw "CreateFileIfNotExist failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::LFileOrDie(const std::string& path) {
    try {
        GalaxyFs fs("");
        std::string out_path;
        auto status = fs.DieFileIfNotExist(path, out_path);
        if (!status.ok()) {
            throw "FileOrDie failed with error " + status.ToString() + '.';
        }
        return out_path;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::impl::LRmFile(const std::string& path) {
    try {
        GalaxyFs fs("");
        auto status = fs.RmFile(path);
        if (!status.ok()) {
            throw "RmFile failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::LRenameFile(const std::string& old_path, const std::string& new_path) {
    try {
        GalaxyFs fs("");
        auto status = fs.RenameFile(old_path, new_path);
        if (!status.ok()) {
            throw "RenameFile failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::LRead(const std::string& path) {
    try {
        GalaxyFs fs("");
        std::string data;
        auto status = fs.Read(path, data);
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

std::map<std::string, std::string> galaxy::client::impl::LReadMultiple(const std::vector<std::string>& paths) {
    GalaxyFs fs("");
    std::map<std::string, std::string> data_map;
    for (const auto& path : paths) {
        std::string data;
        auto status = fs.Read(path, data);
        if (!status.ok()) {
            LOG(ERROR) << "Read " << path <<" failed with error " << status.ToString();
            data_map.insert({path, ""});
        } else {
            data_map.insert({path, data});
        }
    }
    return data_map;
}

void galaxy::client::impl::LWrite(const std::string& path, const std::string& data, const std::string& mode) {
    try {
        GalaxyFs fs("");
        auto status = fs.Write(path, data, mode);
        if (!status.ok()) {
            throw "Write failed with error " + status.ToString() + '.';
        }
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }

}

void galaxy::client::impl::LWriteMultiple(const std::map<std::string, std::string>& path_data_map, const std::string& mode) {
    GalaxyFs fs("");
    for (const auto& val : path_data_map) {
        auto status = fs.Write(val.first, val.second, mode);
        if (!status.ok()) {
            LOG(ERROR) << "Write " << val.first <<" failed with error " << status.ToString();
        }
    }
}

std::string galaxy::client::impl::LGetAttr(const std::string& path) {
    try {
        GalaxyFs fs("");
        struct stat statbuf;
        auto status = fs.GetAttr(path, &statbuf);
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
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RCreateDirIfNotExist(*path_or, mode);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            galaxy::client::impl::LCreateDirIfNotExist(*local_path_or, mode);
        }
    }
}

std::string galaxy::client::DirOrDie(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RDirOrDie(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            return galaxy::client::impl::LDirOrDie(*local_path_or);
        }
    }
}

void galaxy::client::RmDir(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RRmDir(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            galaxy::client::impl::LRmDir(*local_path_or);
        }
    }
}

void galaxy::client::RmDirRecursive(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RRmDirRecursive(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            galaxy::client::impl::LRmDirRecursive(*local_path_or);
        }
    }
}

std::map<std::string, std::string> galaxy::client::ListDirsInDir(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListDirsInDir(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            return galaxy::client::impl::LListDirsInDir(*local_path_or);
        }
    }
}

std::map<std::string, std::string> galaxy::client::ListFilesInDir(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListFilesInDir(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            return galaxy::client::impl::LListFilesInDir(*local_path_or);
        }
    }
}

std::map<std::string, std::string> galaxy::client::ListDirsInDirRecursive(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListDirsInDirRecursive(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            return galaxy::client::impl::LListDirsInDirRecursive(*local_path_or);
        }
    }
}

std::map<std::string, std::string> galaxy::client::ListFilesInDirRecursive(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListFilesInDirRecursive(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            return galaxy::client::impl::LListFilesInDirRecursive(*local_path_or);
        }
    }
}

void galaxy::client::CreateFileIfNotExist(const std::string& path, const int mode) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RCreateFileIfNotExist(*path_or, mode);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            galaxy::client::impl::LCreateFileIfNotExist(*local_path_or, mode);
        }
    }
}

std::string galaxy::client::FileOrDie(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RFileOrDie(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            return galaxy::client::impl::LFileOrDie(*local_path_or);
        }
    }
}

void galaxy::client::RmFile(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RRmFile(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            galaxy::client::impl::LRmFile(*local_path_or);
        }
    }
}

void galaxy::client::RenameFile(const std::string& old_path, const std::string& new_path) {
    absl::StatusOr<std::string> old_path_or = galaxy::util::InitClient(old_path);
    std::string old_cell = absl::GetFlag(FLAGS_fs_cell);
    absl::StatusOr<std::string> new_path_or = galaxy::util::InitClient(new_path);
    std::string new_cell = absl::GetFlag(FLAGS_fs_cell);
    if (old_path_or.ok() && new_path_or.ok()) {
        VLOG(2) << "Using remote mode";
        CHECK_EQ(old_cell, new_cell) << "Files are in different cells.";
        galaxy::client::impl::RRenameFile(*old_path_or, *new_path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_old_path_or = galaxy::util::ConvertToLocalPath(old_path);
        absl::StatusOr<std::string> local_new_path_or = galaxy::util::ConvertToLocalPath(new_path);
        if (!local_old_path_or.ok() || !local_new_path_or.ok()) {
            throw "Invalid Path " + old_path + " and " + new_path;
        } else {
            galaxy::client::impl::LRenameFile(*local_old_path_or, *local_new_path_or);
        }
    }

}

std::string galaxy::client::Read(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RRead(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            return galaxy::client::impl::LRead(*local_path_or);
        }
    }
}

std::map<std::string, std::string> galaxy::client::ReadMultiple(const std::vector<std::string>& paths) {
    std::vector<std::string> remote_paths, local_paths;
    for (const auto& path : paths) {
        absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
        if (path_or.ok()) {
            VLOG(2) << "Using remote mode for " << path;
            remote_paths.push_back(*path_or);
        } else {
            VLOG(1) << "Using local mode for " << path;
            absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
            if (local_path_or.ok()) {
                local_paths.push_back(*local_path_or);
            }
        }
    }

    std::map<std::string, std::string> result;
    if (!remote_paths.empty()) {
        std::map<std::string, std::string> remote_result = galaxy::client::impl::RReadMultiple(remote_paths);
        result.insert(remote_result.begin(), remote_result.end());
    }
    if (!local_paths.empty()) {
        std::map<std::string, std::string> local_result = galaxy::client::impl::LReadMultiple(local_paths);
        result.insert(local_result.begin(), local_result.end());
    }
    return result;
}

void galaxy::client::Write(const std::string& path, const std::string& data, const std::string& mode) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RWrite(*path_or, data, mode);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            galaxy::client::impl::LWrite(*local_path_or, data, mode);
        }
    }
}

void galaxy::client::WriteMultiple(const std::map<std::string, std::string>& path_data_map, const std::string& mode) {
    std::map<std::string, std::string> local_data, remote_data;
    for (const auto& val : path_data_map) {
        absl::StatusOr<std::string> path_or = galaxy::util::InitClient(val.first);
        if (path_or.ok()) {
            VLOG(2) << "Using remote mode for " << val.first;
            remote_data.insert({*path_or, val.second});
        } else {
            VLOG(1) << "Using local mode for " << val.first;
            absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(val.first);
            if (local_path_or.ok()) {
                local_data.insert({*local_path_or, val.second});
            }
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
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RGetAttr(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        absl::StatusOr<std::string> local_path_or = galaxy::util::ConvertToLocalPath(path);
        if (!local_path_or.ok()) {
            throw "Invalid Path " + path;
        } else {
            return galaxy::client::impl::LGetAttr(*local_path_or);
        }
    }
}


std::vector<std::string> galaxy::client::ListCells() {
    absl::StatusOr<std::vector<std::string>> cells_ok = galaxy::util::ParseGlobalConfigAndGetCells();
    if (cells_ok.ok()) {
        return *cells_ok;
    } else {
        VLOG(1) << "Failed to list the cells.";
        return std::vector<std::string>();
    }
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
