#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "cpp/core/galaxy_server.h"
#include "cpp/core/galaxy_fs.h"

using grpc::ServerContext;
using grpc::Status;

using galaxy_schema::FileSystemStatus;
using galaxy_schema::Credential;
using galaxy_schema::ClientInfo;
using galaxy_schema::Attribute;

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

    Status GalaxyServerImpl::GetAttr(ServerContext *context, const GetAttrRequest *request,
                                     GetAttrResponse *reply)
    {
        std::string path = request->name();
        struct stat statbuf;
        GalaxyFs::Instance()->GetAttr(path, &statbuf);

        return Status::OK;
    }

    Status GalaxyServerImpl::CreateDirIfNotExist(ServerContext *context, const CreateDirRequest *request,
                                                 CreateDirResponse *reply)
    {
        return Status::OK;
    }

    Status GalaxyServerImpl::DirOrDie(ServerContext *context, const DirOrDieRequest *request,
                                      DirOrDieResponse *reply)
    {
        return Status::OK;
    }

    Status GalaxyServerImpl::RmDir(ServerContext *context, const RmDirRequest *request,
                                   RmDirResponse *reply)
    {
        return Status::OK;
    }

    Status GalaxyServerImpl::RmDirRecursive(ServerContext *context, const RmDirRecursiveRequest *request,
                                            RmDirRecursiveResponse *reply)
    {
        return Status::OK;
    }

    Status GalaxyServerImpl::CreateFileIfNotExist(ServerContext *context, const CreateFileRequest *request,
                                                  CreateFileResponse *reply)
    {
        return Status::OK;
    }

    Status GalaxyServerImpl::ListDirsInDir(ServerContext *context, const ListDirsInDirRequest *request,
                                           ListDirsInDirResponse *reply)
    {
        return Status::OK;
    }

    Status GalaxyServerImpl::ListFilesInDir(ServerContext *context, const ListFilesInDirRequest *request,
                                            ListFilesInDirResponse *reply)
    {
        return Status::OK;
    }

    Status GalaxyServerImpl::FileOrDie(ServerContext *context, const FileOrDieRequest *request,
                                       FileOrDieResponse *reply)
    {
        return Status::OK;
    }

    Status GalaxyServerImpl::RmFile(ServerContext *context, const RmFileRequest *request,
                                    RmFileResponse *reply)
    {
        return Status::OK;
    }

    Status GalaxyServerImpl::RenameFile(ServerContext *context, const RenameFileRequest *request,
                                        RenameFileResponse *reply)
    {
        return Status::OK;
    }

    Status GalaxyServerImpl::Read(ServerContext *context, const ReadRequest *request,
                                  ReadResponse *reply)
    {
        return Status::OK;
    }

    Status GalaxyServerImpl::Write(ServerContext *context, const WriteRequest *request,
                                   WriteResponse *reply)
    {
        return Status::OK;
    }

} // namespace galaxy