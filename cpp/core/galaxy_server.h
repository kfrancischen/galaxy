#ifndef CPP_CORE_GALAXY_SERVER_H_
#define CPP_CORE_GALAXY_SERVER_H_

#include <grpcpp/grpcpp.h>
#include "absl/status/status.h"
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

        grpc::Status WriteMultiple(grpc::ServerContext *context, const galaxy_schema::WriteMultipleRequest *request,
                                   galaxy_schema::WriteMultipleResponse *reply) override;

        grpc::Status CopyFile(grpc::ServerContext *context, grpc::ServerReader<galaxy_schema::CopyRequest> *request,
                              galaxy_schema::CopyResponse *reply) override;

        grpc::Status CrossCellCall(grpc::ServerContext *context, const galaxy_schema::CrossCellRequest *request,
                                   galaxy_schema::CrossCellResponse *reply) override;

        grpc::Status CheckHealth(grpc::ServerContext *context, const galaxy_schema::HealthCheckRequest *request,
                                 galaxy_schema::HealthCheckResponse *reply) override;

        grpc::Status RemoteExecution(grpc::ServerContext *context, const galaxy_schema::RemoteExecutionRequest *request,
                                     galaxy_schema::RemoteExecutionResponse *reply) override;

        void SetPassword(const std::string &password);

    private:
        std::string password_;
        absl::Status VerifyPassword(const galaxy_schema::Credential &cred);

        grpc::Status GetAttrInternal(grpc::ServerContext *context, const galaxy_schema::GetAttrRequest *request,
                                     galaxy_schema::GetAttrResponse *reply);

        grpc::Status CreateDirIfNotExistInternal(grpc::ServerContext *context, const galaxy_schema::CreateDirRequest *request,
                                                 galaxy_schema::CreateDirResponse *reply);

        grpc::Status DirOrDieInternal(grpc::ServerContext *context, const galaxy_schema::DirOrDieRequest *request,
                                      galaxy_schema::DirOrDieResponse *reply);

        grpc::Status RmDirInternal(grpc::ServerContext *context, const galaxy_schema::RmDirRequest *request,
                                   galaxy_schema::RmDirResponse *reply);

        grpc::Status RmDirRecursiveInternal(grpc::ServerContext *context, const galaxy_schema::RmDirRecursiveRequest *request,
                                            galaxy_schema::RmDirRecursiveResponse *reply);

        grpc::Status ListDirsInDirInternal(grpc::ServerContext *context, const galaxy_schema::ListDirsInDirRequest *request,
                                           galaxy_schema::ListDirsInDirResponse *reply);

        grpc::Status ListFilesInDirInternal(grpc::ServerContext *context, const galaxy_schema::ListFilesInDirRequest *request,
                                            galaxy_schema::ListFilesInDirResponse *reply);

        grpc::Status ListAllInDirRecursiveInternal(grpc::ServerContext *context, const galaxy_schema::ListAllInDirRecursiveRequest *request,
                                                   galaxy_schema::ListAllInDirRecursiveResponse *reply);

        grpc::Status CreateFileIfNotExistInternal(grpc::ServerContext *context, const galaxy_schema::CreateFileRequest *request,
                                                  galaxy_schema::CreateFileResponse *reply);

        grpc::Status FileOrDieInternal(grpc::ServerContext *context, const galaxy_schema::FileOrDieRequest *request,
                                       galaxy_schema::FileOrDieResponse *reply);

        grpc::Status RmFileInternal(grpc::ServerContext *context, const galaxy_schema::RmFileRequest *request,
                                    galaxy_schema::RmFileResponse *reply);

        grpc::Status RenameFileInternal(grpc::ServerContext *context, const galaxy_schema::RenameFileRequest *request,
                                        galaxy_schema::RenameFileResponse *reply);

        grpc::Status ReadInternal(grpc::ServerContext *context, const galaxy_schema::ReadRequest *request,
                                  galaxy_schema::ReadResponse *reply);

        grpc::Status ReadMultipleInternal(grpc::ServerContext *context, const galaxy_schema::ReadMultipleRequest *request,
                                          galaxy_schema::ReadMultipleResponse *reply);

        grpc::Status WriteInternal(grpc::ServerContext *context, const galaxy_schema::WriteRequest *request,
                                   galaxy_schema::WriteResponse *reply);

        grpc::Status WriteMultipleInternal(grpc::ServerContext *context, const galaxy_schema::WriteMultipleRequest *request,
                                           galaxy_schema::WriteMultipleResponse *reply);

        grpc::Status CopyFileInternal(grpc::ServerContext *context, grpc::ServerReader<galaxy_schema::CopyRequest> *request,
                                      galaxy_schema::CopyResponse *reply);

        grpc::Status CrossCellCallInternal(grpc::ServerContext *context, const galaxy_schema::CrossCellRequest *request,
                                           galaxy_schema::CrossCellResponse *reply);

        grpc::Status CheckHealthInternal(grpc::ServerContext *context, const galaxy_schema::HealthCheckRequest *request,
                                         galaxy_schema::HealthCheckResponse *reply);

        grpc::Status RemoteExecutionInternal(grpc::ServerContext *context, const galaxy_schema::RemoteExecutionRequest *request,
                                             galaxy_schema::RemoteExecutionResponse *reply);
    };
} // namespace galaxy

#endif // CPP_CORE_GALAXY_SERVER_H_