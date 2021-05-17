#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <google/protobuf/util/json_util.h>

#include "cpp/client.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/core/galaxy_fs.h"
#include "cpp/util/galaxy_util.h"
#include "cpp/internal/galaxy_client_internal.h"
#include "cpp/internal/galaxy_const.h"
#include "absl/flags/flag.h"
#include "glog/logging.h"

#include "include/rapidjson/document.h"
#include "include/rapidjson/prettywriter.h"
#include "include/rapidjson/stringbuffer.h"

using galaxy_schema::Owner;
using galaxy_schema::FileSystemStatus;
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

using galaxy::GalaxyClientInternal;
using galaxy::GalaxyFs;

GalaxyClientInternal GetChannelClient() {
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    grpc::ChannelArguments ch_args;
    ch_args.SetMaxReceiveMessageSize(-1);
    GalaxyClientInternal client(grpc::CreateCustomChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials(), ch_args));
    return client;
}

void galaxy::client::impl::RCreateDirIfNotExist(const std::string& path, const int mode) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        CreateDirRequest request;
        request.set_name(path);
        request.set_mode(mode);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        CreateDirResponse response = client.CreateDirIfNotExist(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call CreateDirIfNotExist.";
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::RDirOrDie(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        DirOrDieRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        DirOrDieResponse response = client.DirOrDie(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call DirOrDie.";
        return galaxy::util::MapToCellPath(response.name());
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::impl::RRmDir(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        RmDirRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        RmDirResponse response = client.RmDir(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call RmDir.";
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::RRmDirRecursive(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        RmDirRecursiveRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        RmDirRecursiveResponse response = client.RmDirRecursive(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call RmDirRecursive.";
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::vector<std::string> galaxy::client::impl::RListDirsInDir(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        ListDirsInDirRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListDirsInDirResponse response = client.ListDirsInDir(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call ListDirsInDir.";
        std::vector<std::string> result(response.sub_dirs().begin(), response.sub_dirs().end());
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = galaxy::util::MapToCellPath(result[i]);
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}

std::vector<std::string> galaxy::client::impl::RListFilesInDir(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        ListFilesInDirRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListFilesInDirResponse response = client.ListFilesInDir(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call ListFilesInDir.";
        std::vector<std::string> result(response.sub_files().begin(), response.sub_files().end());
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = galaxy::util::MapToCellPath(result[i]);
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}

std::vector<std::string> galaxy::client::impl::RListDirsInDirRecursive(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        ListAllInDirRecursiveRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListAllInDirRecursiveResponse response = client.ListAllInDirRecursive(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call RListDirsInDirRecursive.";
        std::vector<std::string> result(response.sub_dirs().begin(), response.sub_dirs().end());
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = galaxy::util::MapToCellPath(result[i]);
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}

std::vector<std::string> galaxy::client::impl::RListFilesInDirRecursive(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        ListAllInDirRecursiveRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListAllInDirRecursiveResponse response = client.ListAllInDirRecursive(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call RListFilesInDirRecursive.";
        std::vector<std::string> result(response.sub_files().begin(), response.sub_files().end());
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = galaxy::util::MapToCellPath(result[i]);
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}

void galaxy::client::impl::RCreateFileIfNotExist(const std::string& path, const int mode) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        CreateFileRequest request;
        request.set_name(path);
        request.set_mode(mode);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        CreateFileResponse response = client.CreateFileIfNotExist(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call CreateFileIfNotExist.";
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::RFileOrDie(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        FileOrDieRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        FileOrDieResponse response = client.FileOrDie(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call FileOrDie.";
        return galaxy::util::MapToCellPath(response.name());
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::impl::RRmFile(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        RmFileRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        RmFileResponse response = client.RmFile(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call RmFile.";
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::impl::RRenameFile(const std::string& old_path, const std::string& new_path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        RenameFileRequest request;
        request.set_old_name(old_path);
        request.set_new_name(new_path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        RenameFileResponse response = client.RenameFile(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call RenameFile.";
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::RRead(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        ReadRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ReadResponse response = client.Read(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call Read.";
        return response.data();
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

std::map<std::string, std::string> galaxy::client::impl::RReadMultiple(const std::vector<std::string>& paths) {
    GalaxyClientInternal client = GetChannelClient();
    ReadMultipleRequest request;
    std::map<std::string, std::string> result;
    *request.mutable_names() = {paths.begin(), paths.end()};
    request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
    ReadMultipleResponse response = client.ReadMultiple(request);
    for (const auto& pair : response.data()) {
        std::string path = galaxy::util::MapToCellPath(pair.first);
        if (pair.second.status().return_code() != 1) {
            LOG(ERROR) << "Failed to read data for file " << path;
        }
        result.insert({path, pair.second.data()});
    }
    return result;
}

void galaxy::client::impl::RWrite(const std::string& path, const std::string& data, const std::string& mode) {
    GalaxyClientInternal client = GetChannelClient();
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
        CHECK_EQ(status.return_code(), 1) << "Fail to call Write.";
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::impl::RGetAttr(const std::string& path) {
    GalaxyClientInternal client = GetChannelClient();
    try {
        GetAttrRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        GetAttrResponse response = client.GetAttr(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1) << "Fail to call GetAttr.";
        std::string output_attr;
        google::protobuf::util::JsonPrintOptions option;
        option.add_whitespace = true;
        google::protobuf::util::MessageToJsonString(response.attr(), &output_attr, option);
        return output_attr;
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
        CHECK(status.ok()) << "CreateDirIfNotExist failed with error " << status.ToString();
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
        CHECK(status.ok()) << "DirOrDie failed with error " << status.ToString();
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
        CHECK(status.ok()) << "RmDir failed with error " << status.ToString();
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
        CHECK(status.ok()) << "RmDirRecursive failed with error " << status.ToString();
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::vector<std::string> galaxy::client::impl::LListDirsInDir(const std::string& path) {
    try {
        std::vector<std::string> sub_dirs;
        GalaxyFs fs("");
        auto status = fs.ListDirsInDir(path, sub_dirs);
        CHECK(status.ok()) << "ListDirsInDir failed with error " << status.ToString();
        return sub_dirs;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}

std::vector<std::string> galaxy::client::impl::LListFilesInDir(const std::string& path) {
    try {
        std::vector<std::string> sub_files;
        GalaxyFs fs("");
        auto status = fs.ListFilesInDir(path, sub_files);
        CHECK(status.ok()) << "ListFilesInDir failed with error " << status.ToString();
        return sub_files;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}

std::vector<std::string> galaxy::client::impl::LListDirsInDirRecursive(const std::string& path) {
    try {
        std::vector<std::string> sub_files;
        std::vector<std::string> sub_dirs;
        GalaxyFs fs("");
        auto status = fs.ListAllInDirRecursive(path, sub_dirs, sub_files);
        CHECK(status.ok()) << "ListDirsInDirRecursive failed with error " << status.ToString();
        return sub_dirs;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}

std::vector<std::string> galaxy::client::impl::LListFilesInDirRecursive(const std::string& path) {
    try {
        std::vector<std::string> sub_files;
        std::vector<std::string> sub_dirs;
        GalaxyFs fs("");
        auto status = fs.ListAllInDirRecursive(path, sub_dirs, sub_files);
        CHECK(status.ok()) << "ListFilesInDirRecursive failed with error " << status.ToString();
        return sub_files;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}


void galaxy::client::impl::LCreateFileIfNotExist(const std::string& path, const int mode) {
    try {
        GalaxyFs fs("");
        auto status = fs.CreateFileIfNotExist(path, mode);
        CHECK(status.ok()) << "LCreateFileIfNotExist failed with error " << status.ToString();
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
        CHECK(status.ok()) << "FileOrDie failed with error " << status.ToString();
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
        CHECK(status.ok()) << "RmFile failed with error " << status.ToString();
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
        CHECK(status.ok()) << "RenameFile failed with error " << status.ToString();
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
        CHECK(status.ok()) << "Read failed with error " << status.ToString();
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
        CHECK(status.ok()) << "Write failed with error " << status.ToString();
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }

}

std::string galaxy::client::impl::LGetAttr(const std::string& path) {
    try {
        GalaxyFs fs("");
        struct stat statbuf;
        auto status = fs.GetAttr(path, &statbuf);
        CHECK(status.ok()) << "GetAttr failed with error " << status.ToString();
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
        galaxy::client::impl::LCreateDirIfNotExist(path, mode);
    }
}

std::string galaxy::client::DirOrDie(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RDirOrDie(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LDirOrDie(path);
    }
}

void galaxy::client::RmDir(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RRmDir(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        galaxy::client::impl::LRmDir(path);
    }
}

void galaxy::client::RmDirRecursive(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RRmDirRecursive(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        galaxy::client::impl::LRmDirRecursive(path);
    }
}

std::vector<std::string> galaxy::client::ListDirsInDir(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListDirsInDir(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LListDirsInDir(path);
    }
}

std::vector<std::string> galaxy::client::ListFilesInDir(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListFilesInDir(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LListFilesInDir(path);
    }
}

std::vector<std::string> galaxy::client::ListDirsInDirRecursive(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListDirsInDirRecursive(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LListDirsInDirRecursive(path);
    }
}

std::vector<std::string> galaxy::client::ListFilesInDirRecursive(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RListFilesInDirRecursive(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LListFilesInDirRecursive(path);
    }
}

void galaxy::client::CreateFileIfNotExist(const std::string& path, const int mode) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RCreateFileIfNotExist(*path_or, mode);
    } else {
        VLOG(1) << "Using local mode";
        galaxy::client::impl::LCreateFileIfNotExist(path, mode);
    }
}

std::string galaxy::client::FileOrDie(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RFileOrDie(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LFileOrDie(path);
    }
}

void galaxy::client::RmFile(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        galaxy::client::impl::RRmFile(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        galaxy::client::impl::LRmFile(path);
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
        galaxy::client::impl::LRenameFile(old_path, new_path);
    }

}

std::string galaxy::client::Read(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RRead(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LRead(path);
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
            local_paths.push_back(path);
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
        galaxy::client::impl::LWrite(path, data, mode);
    }
}

std::string galaxy::client::GetAttr(const std::string& path) {
    absl::StatusOr<std::string> path_or = galaxy::util::InitClient(path);
    if (path_or.ok()) {
        VLOG(2) << "Using remote mode";
        return galaxy::client::impl::RGetAttr(*path_or);
    } else {
        VLOG(1) << "Using local mode";
        return galaxy::client::impl::LGetAttr(path);
    }
}
