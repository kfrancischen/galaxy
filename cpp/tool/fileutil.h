#ifndef CPP_LOCAL_FILEUTIL_H_
#define CPP_LOCAL_FILEUTIL_H_

#include <grpcpp/grpcpp.h>
#include "schema/fileserver.grpc.pb.h"

namespace galaxy
{
    namespace impl
    {
        class GalaxyFileutil
        {
        public:
            GalaxyFileutil(std::shared_ptr<grpc::Channel> channel) : stub_(galaxy_schema::FileSystem::NewStub(channel)) {}
            galaxy_schema::DownloadResponse DownloadFile(const galaxy_schema::DownloadRequest &request);
            galaxy_schema::DownloadResponse CopyFile(const galaxy_schema::DownloadRequest &request);
            galaxy_schema::UploadResponse UploadFile(const galaxy_schema::UploadRequest &request);

        private:
            std::unique_ptr<galaxy_schema::FileSystem::Stub> stub_;
        };
    } // namespace internal


    void LsCmd(const std::string& path);
    void GetFileCmd(const std::string& from_path, const std::string& to_path);
    void CopyFileCmd(const std::string& from_path, const std::string& to_path);
    void MoveFileCmd(const std::string& from_path, const std::string& to_path);
    void UploadFileCmd(const std::string& from_path, const std::string& to_path);

} // namespace galaxy

#endif // CPP_LOCAL_FILEUTIL_H_