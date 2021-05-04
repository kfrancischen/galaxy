#ifndef CPP_CORE_GALAXY_SERVER_H_
#define CPP_CORE_GALAXY_SERVER_H_

#include <grpcpp/grpcpp.h>
#include "schema/fileserver.grpc.pb.h"

namespace galaxy
{
    class GalaxyServerImpl final : public galaxy_schema::FileSystem::Service
    {
    public:
        grpc::Status GetAttr(grpc::ServerContext *context, const galaxy_schema::GetAttrRequest *request,
                             galaxy_schema::GetAttrResponse *reply) override;

        grpc::Status CreateDirIfNotExist(grpc::ServerContext *context, const galaxy_schema::CreateDirRequest *request,
                                         galaxy_schema::CreateDirResponse *reply) override;

        grpc::Status DirOrDie(grpc::ServerContext *context, const galaxy_schema::DirOrDieRequest *request,
                              galaxy_schema::DirOrDieResponse *reply) override;

        grpc::Status RmDir(grpc::ServerContext *context, const galaxy_schema::RmDirRequest *request,
                           galaxy_schema::RmDirResponse *reply) override;

        grpc::Status RmDirRecursive(grpc::ServerContext *context, const galaxy_schema::RmDirRecursiveRequest *request,
                                    galaxy_schema::RmDirRecursiveResponse *reply) override;

        grpc::Status ListDirsInDir(grpc::ServerContext *context, const galaxy_schema::ListDirsInDirRequest *request,
                                   galaxy_schema::ListDirsInDirResponse *reply) override;

        grpc::Status ListFilesInDir(grpc::ServerContext *context, const galaxy_schema::ListFilesInDirRequest *request,
                                    galaxy_schema::ListFilesInDirResponse *reply) override;

        grpc::Status CreateFileIfNotExist(grpc::ServerContext *context, const galaxy_schema::CreateFileRequest *request,
                                          galaxy_schema::CreateFileResponse *reply) override;

        grpc::Status FileOrDie(grpc::ServerContext *context, const galaxy_schema::FileOrDieRequest *request,
                               galaxy_schema::FileOrDieResponse *reply) override;

        grpc::Status RmFile(grpc::ServerContext *context, const galaxy_schema::RmFileRequest *request,
                            galaxy_schema::RmFileResponse *reply) override;

        grpc::Status RenameFile(grpc::ServerContext *context, const galaxy_schema::RenameFileRequest *request,
                                galaxy_schema::RenameFileResponse *reply) override;

        grpc::Status Read(grpc::ServerContext *context, const galaxy_schema::ReadRequest *request,
                          galaxy_schema::ReadResponse *reply) override;

        grpc::Status Write(grpc::ServerContext *context, const galaxy_schema::WriteRequest *request,
                           galaxy_schema::WriteResponse *reply) override;
    };
} // namespace galaxy

#endif // CPP_CORE_GALAXY_SERVER_H_