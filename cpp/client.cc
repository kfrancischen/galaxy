#include "cpp/client.h"
#include "cpp/core/galaxy_flag.h"
#include "absl/flags/flag.h"

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

void galaxy::client::CreateDirIfNotExist(const std::string& path, const int mode) {
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        CreateDirRequest request;
        request.set_name(path);
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
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        DirOrDieRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        DirOrDieResponse response = client.DirOrDie(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
        return response.name();
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::RmDir(const std::string& path) {
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        RmDirRequest request;
        request.set_name(path);
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
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        RmDirRecursiveRequest request;
        request.set_name(path);
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
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        ListDirsInDirRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListDirsInDirResponse response = client.ListDirsInDir(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
        std::vector<std::string> result(response.sub_dirs().begin(), response.sub_dirs().end());
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}

std::vector<std::string> galaxy::client::ListFilesInDir(const std::string& path) {
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        ListFilesInDirRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        ListFilesInDirResponse response = client.ListFilesInDir(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
        std::vector<std::string> result(response.sub_files().begin(), response.sub_files().end());
        return result;
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return std::vector<std::string>();
    }
}

void galaxy::client::CreateFileIfNotExist(const std::string& path, const int mode) {
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        CreateFileRequest request;
        request.set_name(path);
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
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        FileOrDieRequest request;
        request.set_name(path);
        request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
        FileOrDieResponse response = client.FileOrDie(request);
        FileSystemStatus status = response.status();
        CHECK_EQ(status.return_code(), 1);
        return response.name();
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
        return "";
    }
}

void galaxy::client::RmFile(const std::string& path) {
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        RmFileRequest request;
        request.set_name(path);
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
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        RenameFileRequest request;
        request.set_old_name(old_path);
        request.set_new_name(new_path);
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
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        ReadRequest request;
        request.set_name(path);
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
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
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
        CHECK_EQ(status.return_code(), 1);
    }
    catch (std::string errorMsg)
    {
        LOG(ERROR) << errorMsg;
    }
}

std::string galaxy::client::GetAttr(const std::string& path) {
    GalaxyClientInternal client(grpc::CreateChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials()));
    try {
        GetAttrRequest request;
        request.set_name(path);
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
