#include <iostream>
#include <fstream>
#include <string>
#include "cpp/internal/galaxy_client_internal.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/internal/galaxy_const.h"
#include "glog/logging.h"
#include "absl/flags/flag.h"

using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReader;
using grpc::ClientWriter;

using galaxy_schema::CopyRequest;
using galaxy_schema::CopyResponse;
using galaxy_schema::FileSystemStatus;
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

namespace galaxy
{
    GetAttrResponse GalaxyClientInternal::GetAttr(const GetAttrRequest &request)
    {
        GetAttrResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->GetAttr(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    CreateDirResponse GalaxyClientInternal::CreateDirIfNotExist(const CreateDirRequest &request)
    {
        CreateDirResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->CreateDirIfNotExist(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    CopyResponse GalaxyClientInternal::CopyFile(const CopyRequest &request)
    {
        CopyResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        std::unique_ptr<ClientReader<CopyResponse>> reader(stub_->CopyFile(&context, request));
         while (reader->Read(&reply))
        {
            if (reply.status().return_code() != 1) {
                LOG(ERROR) << reply.status().return_message();
                throw reply.status().return_message();
            }
        }
        Status status = reader->Finish();
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    CopyResponse GalaxyClientInternal::MoveFile(const CopyRequest &request)
    {
        CopyResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        std::unique_ptr<ClientReader<CopyResponse>> reader(stub_->MoveFile(&context, request));
         while (reader->Read(&reply))
        {
            if (reply.status().return_code() != 1) {
                LOG(ERROR) << reply.status().return_message();
                throw reply.status().return_message();
            }
        }
        Status status = reader->Finish();
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }



    CrossCellResponse GalaxyClientInternal::CrossCellCall(const CrossCellRequest &request)
    {
        CrossCellResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->CrossCellCall(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }


    DirOrDieResponse GalaxyClientInternal::DirOrDie(const DirOrDieRequest &request)
    {
        DirOrDieResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->DirOrDie(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    RmDirResponse GalaxyClientInternal::RmDir(const RmDirRequest &request)
    {
        RmDirResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->RmDir(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    RmDirRecursiveResponse GalaxyClientInternal::RmDirRecursive(const RmDirRecursiveRequest &request)
    {
        RmDirRecursiveResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->RmDirRecursive(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    ListDirsInDirResponse GalaxyClientInternal::ListDirsInDir(const ListDirsInDirRequest &request)
    {
        ListDirsInDirResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->ListDirsInDir(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    ListFilesInDirResponse GalaxyClientInternal::ListFilesInDir(const ListFilesInDirRequest &request)
    {
        ListFilesInDirResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->ListFilesInDir(&context, request, &reply);
        if (status.ok())  {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    ListAllInDirRecursiveResponse GalaxyClientInternal::ListAllInDirRecursive(const ListAllInDirRecursiveRequest &request)
    {
        ListAllInDirRecursiveResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->ListAllInDirRecursive(&context, request, &reply);
        if (status.ok())  {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    CreateFileResponse GalaxyClientInternal::CreateFileIfNotExist(const CreateFileRequest &request)
    {
        CreateFileResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->CreateFileIfNotExist(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    FileOrDieResponse GalaxyClientInternal::FileOrDie(const FileOrDieRequest &request)
    {
        FileOrDieResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->FileOrDie(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    RmFileResponse GalaxyClientInternal::RmFile(const RmFileRequest &request)
    {
        RmFileResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->RmFile(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }
    RenameFileResponse GalaxyClientInternal::RenameFile(const RenameFileRequest &request) {
        RenameFileResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->RenameFile(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    ReadResponse GalaxyClientInternal::Read(const ReadRequest &request)
    {
        ReadResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->Read(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    ReadMultipleResponse GalaxyClientInternal::ReadMultiple(const ReadMultipleRequest &request)
    {
        ReadMultipleResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->ReadMultiple(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    WriteResponse GalaxyClientInternal::Write(const WriteRequest &request)
    {
        WriteResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->Write(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    WriteMultipleResponse GalaxyClientInternal::WriteMultiple(const WriteMultipleRequest &request)
    {
        WriteMultipleResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->WriteMultiple(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    HealthCheckResponse GalaxyClientInternal::CheckHealth(const HealthCheckRequest &request)
    {
        HealthCheckResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->CheckHealth(&context, request, &reply);
        if (status.ok()) {
            return reply;
        } else {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }
} // namespace galaxy
