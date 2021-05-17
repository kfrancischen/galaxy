#ifndef CPP_CORE_GALAXY_SERVER_H_
#define CPP_CORE_GALAXY_SERVER_H_

#include <grpcpp/grpcpp.h>
#include "schema/fileserver.grpc.pb.h"
#include "absl/status/status.h"

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

        grpc::Status ListAllInDirRecursive(grpc::ServerContext *context, const galaxy_schema::ListAllInDirRecursiveRequest *request,
                                    galaxy_schema::ListAllInDirRecursiveResponse *reply) override;

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

        grpc::Status ReadMultiple(grpc::ServerContext *context, const galaxy_schema::ReadMultipleRequest *request,
                          galaxy_schema::ReadMultipleResponse *reply) override;

        grpc::Status Write(grpc::ServerContext *context, const galaxy_schema::WriteRequest *request,
                           galaxy_schema::WriteResponse *reply) override;

        grpc::Status DownloadFile(grpc::ServerContext *context, const galaxy_schema::DownloadRequest *request,
                          grpc::ServerWriter<galaxy_schema::DownloadResponse> *reply) override;

        grpc::Status UploadFile(grpc::ServerContext *context, grpc::ServerReader<galaxy_schema::UploadRequest> *request,
                           galaxy_schema::UploadResponse *reply) override;

        void SetPassword(const std::string& password);

        absl::Status VerifyPassword(const galaxy_schema::Credential& cred);

    private:
        std::string password_;
    };
} // namespace galaxy

#endif // CPP_CORE_GALAXY_SERVER_H_