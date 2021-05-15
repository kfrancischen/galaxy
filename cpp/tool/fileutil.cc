#include <iostream>
#include <fstream>
#include <string>

#include "cpp/tool/fileutil.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/internal/galaxy_const.h"
#include "cpp/util/galaxy_util.h"
#include "cpp/client.h"
#include "absl/flags/flag.h"
#include "glog/logging.h"

using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReader;
using grpc::ClientWriter;

using galaxy_schema::FileSystemStatus;
using galaxy_schema::DownloadRequest;
using galaxy_schema::DownloadResponse;
using galaxy_schema::UploadRequest;
using galaxy_schema::UploadResponse;

namespace galaxy
{
    DownloadResponse impl::GalaxyFileutil::DownloadFile(const DownloadRequest &request)
    {
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        std::unique_ptr<ClientReader<DownloadResponse>> reader(stub_->DownloadFile(&context, request));

        DownloadResponse reply;
        std::ofstream outfile(request.to_name(), std::ofstream::app | std::ofstream::binary);
        while (reader->Read(&reply))
        {
            outfile << reply.data();
        }
        outfile.close();
        Status status = reader->Finish();
        if (status.ok())
        {
            DownloadResponse response;
            FileSystemStatus status;
            status.set_return_code(1);
            response.mutable_status()->CopyFrom(status);
            return response;
        }
        else
        {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    UploadResponse impl::GalaxyFileutil::UploadFile(const UploadRequest &request)
    {
        ClientContext context;
        UploadResponse reply;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        std::unique_ptr<ClientWriter<UploadRequest>> writer(stub_->UploadFile(&context, &reply));
        std::ifstream infile(request.from_name(), std::ifstream::binary);
        std::vector<char> buffer(galaxy::constant::kChunkSize + 1, 0);
        while (infile)
        {
            infile.read(buffer.data(), sizeof(buffer));
            std::streamsize s = ((infile) ? galaxy::constant::kChunkSize : infile.gcount());
            buffer[s] = 0;
            UploadRequest sub_request;
            sub_request.set_from_name(request.from_name());
            sub_request.set_to_name(request.to_name());
            sub_request.mutable_cred()->set_password(request.cred().password());
            sub_request.set_data(std::string(buffer.data()));
            if (!writer->Write(sub_request))
            {
                break;
            }
        }
        infile.close();
        Status status = writer->Finish();
        if (status.ok())
        {
            return reply;
        }
        else
        {
            LOG(ERROR) << status.error_code() << ": " << status.error_message();
            throw status.error_message();
        }
    }

    impl::GalaxyFileutil GetFileutilClient() {
        absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(false);
        CHECK(result.ok()) << "Fail to parse the global config.";
        grpc::ChannelArguments ch_args;
        ch_args.SetMaxReceiveMessageSize(-1);
        impl::GalaxyFileutil client(grpc::CreateCustomChannel(absl::GetFlag(FLAGS_fs_address), grpc::InsecureChannelCredentials(), ch_args));
        return client;
    }

    void GetFileCmd(const std::string& from_path, const std::string& to_path) {
        try {
            absl::StatusOr<std::string> from_path_or = galaxy::util::InitClient(from_path);
            absl::StatusOr<std::string> to_path_or = galaxy::util::InitClient(to_path);
            CHECK(from_path_or.ok() && !to_path_or.ok()) << "Please make sure the first path is remote and the second path is local.";
            auto client = GetFileutilClient();
            DownloadRequest request;
            request.set_from_name(*from_path_or);
            request.set_to_name(to_path);
            request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
            DownloadResponse response = client.DownloadFile(request);
            FileSystemStatus status = response.status();
            CHECK_EQ(status.return_code(), 1) << "Fail to call Get cmd.";
        }
        catch (std::string errorMsg)
        {
            LOG(ERROR) << errorMsg;
        }
    }

    void UploadFileCmd(const std::string& from_path, const std::string& to_path) {
        try {
            absl::StatusOr<std::string> from_path_or = util::InitClient(from_path);
            absl::StatusOr<std::string> to_path_or = util::InitClient(to_path);
            CHECK(!from_path_or.ok() && to_path_or.ok()) << "Please make sure the first path is local and the second path is remote.";
            auto client = GetFileutilClient();
            UploadRequest request;
            request.set_from_name(from_path);
            request.set_to_name(*to_path_or);
            request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
            UploadResponse response = client.UploadFile(request);
            FileSystemStatus status = response.status();
            CHECK_EQ(status.return_code(), 1) << "Fail to call Upload cmd.";
        }
        catch (std::string errorMsg)
        {
            LOG(ERROR) << errorMsg;
        }
    }

    void LsCmd(const std::string& path) {
        std::vector<std::string> sub_dirs = client::ListDirsInDir(path);
        std::vector<std::string> sub_files = client::ListFilesInDir(path);
        for (const auto& dir : sub_dirs) {
            std::cout << "\tDirectory Entry: " << dir << std::endl;
        }
        for (const auto& file : sub_files) {
            std::cout << "\tFile Entry:      " << file << std::endl;
        }
    }

    void CopyFileCmd(const std::string& from_path, const std::string& to_path) {
    }

    void MvFileCmd(const std::string& from_path, const std::string& to_path) {
    }
}
