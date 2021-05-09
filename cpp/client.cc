#include <algorithm>

#include "cpp/client.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/core/galaxy_fs.h"
#include "cpp/util/galaxy_util.h"
#include "cpp/internal/galaxy_client_internal.h"
#include "cpp/internal/galaxy_const.h"
#include "absl/flags/flag.h"
#include "glog/logging.h"

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
using galaxy_schema::ReadRequest;
using galaxy_schema::ReadResponse;
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

absl::StatusOr<std::string> InitClient(const std::string& path) {
    absl::StatusOr<std::pair<std::string, std::string>> cell_and_path = galaxy::util::GetCellAndPathFromPath(path);
    if (!cell_and_path.ok()) {
        return absl::InternalError("Wrong format of path.");
    } else {
        absl::SetFlag(&FLAGS_fs_cell, (*cell_and_path).first);
        return (*cell_and_path).second;
    }
}

std::string MapToCellPath(const std::string& path) {
    std::string separator(1, galaxy::constant::kSeparator);
    std::string cell_suffix(galaxy::constant::kCellSuffix);
    std::string cell_prefix = separator + absl::GetFlag(FLAGS_fs_cell) + cell_suffix;
    std::string out_path(path);
    out_path.replace(0, absl::GetFlag(FLAGS_fs_root).length(), cell_prefix);
    return out_path;
}


void galaxy::client::CreateDirIfNotExist(const std::string& path, const int mode) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        CreateDirRequest request;
        request.set_name(*path_or);
        request.set_mode(mode);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        CreateDirResponse response = client.CreateDirIfNotExist(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::DirOrDie(const std::string& path) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        DirOrDieRequest request;
        request.set_name(*path_or);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        DirOrDieResponse response = client.DirOrDie(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
        return MapToCellPath(response.name());
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::RmDir(const std::string& path) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        RmDirRequest request;
        request.set_name(*path_or);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        RmDirResponse response = client.RmDir(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::RmDirRecursive(const std::string& path) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        RmDirRecursiveRequest request;
        request.set_name(*path_or);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        RmDirRecursiveResponse response = client.RmDirRecursive(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::vector<std::string> galaxy::client::ListDirsInDir(const std::string& path) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        ListDirsInDirRequest request;
        request.set_name(*path_or);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListDirsInDirResponse response = client.ListDirsInDir(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
        std::vector<std::string> result(response.sub_dirs().begin(), response.sub_dirs().end());
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = MapToCellPath(result[i]);
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}

std::vector<std::string> galaxy::client::ListFilesInDir(const std::string& path) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        ListFilesInDirRequest request;
        request.set_name(*path_or);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListFilesInDirResponse response = client.ListFilesInDir(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
        std::vector<std::string> result(response.sub_files().begin(), response.sub_files().end());
        for (size_t i = 0; i < result.size(); i++) {
            result[i] = MapToCellPath(result[i]);
        }
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}

void galaxy::client::CreateFileIfNotExist(const std::string& path, const int mode) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        CreateFileRequest request;
        request.set_name(*path_or);
        request.set_mode(mode);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        CreateFileResponse response = client.CreateFileIfNotExist(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::FileOrDie(const std::string& path) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        FileOrDieRequest request;
        request.set_name(*path_or);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        FileOrDieResponse response = client.FileOrDie(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
        return MapToCellPath(response.name());
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::RmFile(const std::string& path) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        RmFileRequest request;
        request.set_name(*path_or);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        RmFileResponse response = client.RmFile(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

void galaxy::client::RenameFile(const std::string& old_path, const std::string& new_path) {
    absl::StatusOr<std::string> old_path_or = InitClient(old_path);
    CHECK(old_path_or.ok()) << "Wrong format of path " << old_path;
    absl::StatusOr<std::string> new_path_or = InitClient(new_path);
    CHECK(new_path_or.ok()) << "Wrong format of path " << new_path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        RenameFileRequest request;
        request.set_old_name(*old_path_or);
        request.set_new_name(*new_path_or);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        RenameFileResponse response = client.RenameFile(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::Read(const std::string& path) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        ReadRequest request;
        request.set_name(*path_or);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ReadResponse response = client.Read(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
        return response.data();
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::Write(const std::string& path, const std::string& data, const std::string& mode) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    CHECK(mode == "a" || mode == "w");
    try {
        WriteRequest request;
        request.set_name(*path_or);
        request.set_data(data);
        if (mode == "a") {
            request.set_mode(WriteMode::APPEND);
        } else {
            request.set_mode(WriteMode::OVERWRITE);
        }
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        WriteResponse response = client.Write(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::GetAttr(const std::string& path) {
    absl::StatusOr<std::string> path_or = InitClient(path);
    CHECK(path_or.ok()) << "Wrong format of path " << path;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
    CHECK(result.ok()) << "Fail to parse the global config.";
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        GetAttrRequest request;
        request.set_name(*path_or);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        GetAttrResponse response = client.GetAttr(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
        return response.attr().DebugString();
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::LCreateDirIfNotExist(const std::string& path, const int mode) {
    GalaxyFs fs("");
    auto status = fs.CreateDirIfNotExist(path, mode);
    if (!status.ok()) {
        LOG(ERROR) << "CreateDirIfNotExist failed with error " << status.ToString();
    }
}

std::string galaxy::client::LDirOrDie(const std::string& path) {
    GalaxyFs fs("");
    std::string out_path;
    auto status = fs.DieDirIfNotExist(path, out_path);
    if (!status.ok()) {
        LOG(ERROR) << "DirOrDie failed with error " << status.ToString();
    }
    return out_path;
}

void galaxy::client::LRmDir(const std::string& path) {
    GalaxyFs fs("");
    auto status = fs.RmDir(path);
    if (!status.ok()) {
        LOG(ERROR) << "RmDir failed with error " << status.ToString();
    }
}

void galaxy::client::LRmDirRecursive(const std::string& path) {
    GalaxyFs fs("");
    auto status = fs.RmDirRecursive(path);
    if (!status.ok()) {
        LOG(ERROR) << "RmDirRecursive failed with error " << status.ToString();
    }
}

std::vector<std::string> galaxy::client::LListDirsInDir(const std::string& path) {
    std::vector<std::string> sub_dirs;
    GalaxyFs fs("");
    auto status = fs.ListDirsInDir(path, sub_dirs);
    if (!status.ok()) {
        LOG(ERROR) << "ListDirsInDir failed with error " << status.ToString();
    }
    return sub_dirs;
}

std::vector<std::string> galaxy::client::LListFilesInDir(const std::string& path) {
    std::vector<std::string> sub_files;
    GalaxyFs fs("");
    auto status = fs.ListFilesInDir(path, sub_files);
    if (!status.ok()) {
        LOG(ERROR) << "ListFilesInDir failed with error " << status.ToString();
    }
    return sub_files;
}

void galaxy::client::LCreateFileIfNotExist(const std::string& path, const int mode) {
    GalaxyFs fs("");
    auto status = fs.CreateFileIfNotExist(path, mode);
    if (!status.ok()) {
        LOG(ERROR) << "CreateFileIfNotExist failed with error " << status.ToString();
    }
}

std::string galaxy::client::LFileOrDie(const std::string& path) {
    GalaxyFs fs("");
    std::string out_path;
    auto status = fs.DieFileIfNotExist(path, out_path);
    if (!status.ok()) {
        LOG(ERROR) << "FileOrDie failed with error " << status.ToString();
    }
    return out_path;
}

void galaxy::client::LRmFile(const std::string& path) {
    GalaxyFs fs("");
    auto status = fs.RmFile(path);
    if (!status.ok()) {
        LOG(ERROR) << "RmFile failed with error " << status.ToString();
    }
}

void galaxy::client::LRenameFile(const std::string& old_path, const std::string& new_path) {
    GalaxyFs fs("");
    auto status = fs.RenameFile(old_path, new_path);
    if (!status.ok()) {
        LOG(ERROR) << "RenameFile failed with error " << status.ToString();
    }
}

std::string galaxy::client::LRead(const std::string& path) {
    GalaxyFs fs("");
    std::string data;
    auto status = fs.Read(path, data);
    if (!status.ok()) {
        LOG(ERROR) << "Read failed with error " << status.ToString();
    }
    return data;
}

void galaxy::client::LWrite(const std::string& path, const std::string& data, const std::string& mode) {
    GalaxyFs fs("");
    auto status = fs.Write(path, data, mode);
    if (!status.ok()) {
        LOG(ERROR) << "Write failed with error " << status.ToString();
    }
}
