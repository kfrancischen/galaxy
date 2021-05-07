#include <string>
#include "glog/logging.h"
#include "cpp/internal/galaxy_client_internal.h"

using grpc::ClientContext;
using grpc::Status;

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

namespace galaxy
{
    GetAttrResponse GalaxyClientInternal::GetAttr(const GetAttrRequest &request)
    {
        GetAttrResponse reply;
        ClientContext context;
        Status status = stub_->GetAttr(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }

    CreateDirResponse GalaxyClientInternal::CreateDirIfNotExist(const CreateDirRequest &request)
    {
        CreateDirResponse reply;
        ClientContext context;
        Status status = stub_->CreateDirIfNotExist(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }

    DirOrDieResponse GalaxyClientInternal::DirOrDie(const DirOrDieRequest &request)
    {
        DirOrDieResponse reply;
        ClientContext context;
        Status status = stub_->DirOrDie(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }

    RmDirResponse GalaxyClientInternal::RmDir(const RmDirRequest &request)
    {
        RmDirResponse reply;
        ClientContext context;
        Status status = stub_->RmDir(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }

    RmDirRecursiveResponse GalaxyClientInternal::RmDirRecursive(const RmDirRecursiveRequest &request)
    {
        RmDirRecursiveResponse reply;
        ClientContext context;
        Status status = stub_->RmDirRecursive(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }

    ListDirsInDirResponse GalaxyClientInternal::ListDirsInDir(const ListDirsInDirRequest &request)
    {
        ListDirsInDirResponse reply;
        ClientContext context;
        Status status = stub_->ListDirsInDir(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }

    ListFilesInDirResponse GalaxyClientInternal::ListFilesInDir(const ListFilesInDirRequest &request)
    {
        ListFilesInDirResponse reply;
        ClientContext context;
        Status status = stub_->ListFilesInDir(&context, request, &reply);
        if (status.ok())  {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }

    CreateFileResponse GalaxyClientInternal::CreateFileIfNotExist(const CreateFileRequest &request)
    {
        CreateFileResponse reply;
        ClientContext context;
        Status status = stub_->CreateFileIfNotExist(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }

    FileOrDieResponse GalaxyClientInternal::FileOrDie(const FileOrDieRequest &request)
    {
        FileOrDieResponse reply;
        ClientContext context;
        Status status = stub_->FileOrDie(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }

    RmFileResponse GalaxyClientInternal::RmFile(const RmFileRequest &request)
    {
        RmFileResponse reply;
        ClientContext context;
        Status status = stub_->RmFile(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }

    ReadResponse GalaxyClientInternal::Read(const ReadRequest &request)
    {
        ReadResponse reply;
        ClientContext context;
        Status status = stub_->Read(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }

    WriteResponse GalaxyClientInternal::Write(const WriteRequest &request)
    {
        WriteResponse reply;
        ClientContext context;
        Status status = stub_->Write(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " + status.error_message();
            throw status.error_message();
        }
    }
} // namespace galaxy
