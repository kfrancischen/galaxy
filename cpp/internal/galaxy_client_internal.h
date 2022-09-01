#ifndef CPP_INTERNAL_GALAXY_CLIENT_INTERNAL_H_
#define CPP_INTERNAL_GALAXY_CLIENT_INTERNAL_H_

#include <grpcpp/grpcpp.h>
#include "schema/fileserver.grpc.pb.h"

namespace galaxy
{
    class GalaxyClientInternal
    {
    public:
        GalaxyClientInternal(std::shared_ptr<grpc::Channel> channel) : stub_(galaxy_schema::FileSystem::NewStub(channel)) {}

        galaxy_schema::GetAttrResponse GetAttr(const galaxy_schema::GetAttrRequest &request);
        galaxy_schema::CreateDirResponse CreateDirIfNotExist(const galaxy_schema::CreateDirRequest &request);
        galaxy_schema::CopyResponse CopyFile(const galaxy_schema::CopyRequest &request);
        galaxy_schema::CrossCellResponse CrossCellCall(const galaxy_schema::CrossCellRequest& request);
        galaxy_schema::DirOrDieResponse DirOrDie(const galaxy_schema::DirOrDieRequest &request);
        galaxy_schema::RmDirResponse RmDir(const galaxy_schema::RmDirRequest &request);
        galaxy_schema::RmDirRecursiveResponse RmDirRecursive(const galaxy_schema::RmDirRecursiveRequest &request);
        galaxy_schema::ListDirsInDirResponse ListDirsInDir(const galaxy_schema::ListDirsInDirRequest &request);
        galaxy_schema::ListFilesInDirResponse ListFilesInDir(const galaxy_schema::ListFilesInDirRequest &request);
        galaxy_schema::ListAllInDirRecursiveResponse ListAllInDirRecursive(const galaxy_schema::ListAllInDirRecursiveRequest &request);
        galaxy_schema::CreateFileResponse CreateFileIfNotExist(const galaxy_schema::CreateFileRequest &request);
        galaxy_schema::FileOrDieResponse FileOrDie(const galaxy_schema::FileOrDieRequest &request);
        galaxy_schema::RmFileResponse RmFile(const galaxy_schema::RmFileRequest &request);
        galaxy_schema::RenameFileResponse RenameFile(const galaxy_schema::RenameFileRequest &request);
        galaxy_schema::ReadResponse Read(const galaxy_schema::ReadRequest &request);
        galaxy_schema::ReadMultipleResponse ReadMultiple(const galaxy_schema::ReadMultipleRequest &request);
        galaxy_schema::WriteMultipleResponse WriteMultiple(const galaxy_schema::WriteMultipleRequest &request);
        galaxy_schema::WriteResponse Write(const galaxy_schema::WriteRequest &request);
        galaxy_schema::HealthCheckResponse CheckHealth(const galaxy_schema::HealthCheckRequest& request);

    private:
        std::unique_ptr<galaxy_schema::FileSystem::Stub> stub_;
    };

} // namespace galaxy

#endif // CPP_INTERNAL_GALAXY_CLIENT_INTERNAL_H_