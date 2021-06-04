#include <iostream>
#include <fstream>
#include <string>
#include <thread>

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
        std::ofstream outfile(request.to_name(), std::ofstream::binary);
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

    DownloadResponse impl::GalaxyFileutil::CopyFile(const DownloadRequest &request)
    {
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        std::unique_ptr<ClientReader<DownloadResponse>> reader(stub_->DownloadFile(&context, request));
        DownloadResponse reply;
        std::string from_cell = absl::GetFlag(FLAGS_fs_cell);
        while (reader->Read(&reply))
        {
            client::Write(request.to_name(), reply.data(), "a");
            absl::SetFlag(&FLAGS_fs_cell, from_cell);
        }
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
        std::vector<char> buffer(galaxy::constant::kChunkSize, 0);
        while (!infile.eof())
        {
            infile.read(buffer.data(), sizeof(buffer));
            std::streamsize s = infile.gcount();
            UploadRequest sub_request;
            sub_request.set_from_name(request.from_name());
            sub_request.set_to_name(request.to_name());
            sub_request.mutable_cred()->set_password(request.cred().password());
            sub_request.set_data(std::string(buffer.begin(), buffer.begin() + s));
            if (!writer->Write(sub_request))
            {
                break;
            }
        }
        infile.close();
        writer->WritesDone();
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
            std::cout << "Done getting file from " << from_path << " to " << to_path << std::endl;
        }
        catch (std::string errorMsg)
        {
            LOG(FATAL) << errorMsg;
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
            std::cout << "Done uploading file from " << from_path << " to " << to_path << std::endl;
        }
        catch (std::string errorMsg)
        {
            LOG(FATAL) << errorMsg;
        }
    }

    void LsCmd(const std::string& path) {
        std::map<std::string, std::string> sub_dirs = client::ListDirsInDir(path);
        std::map<std::string, std::string> sub_files = client::ListFilesInDir(path);
        for (const auto& dir : sub_dirs) {
            std::cout << "\tDirectory Entry: " << dir.first << std::endl;
        }
        for (const auto& file : sub_files) {
            std::cout << "\tFile Entry:      " << file.first << std::endl;
        }
    }

    void RmCmd(const std::string& path, bool recursive) {
        if (recursive) {
            client::RmDirRecursive(path);
        } else {
            client::RmDir(path);
        }
        client::RmFile(path);
        std::cout << "Done removing " << path << std::endl;
    }

    void ListCellsCmd() {
        std::vector<std::string> cells = client::ListCells();
        std::cout << "In total " << cells.size() << " cells:" << std::endl;
        for (const auto& cell : cells) {
            std::cout << "\t[cell]: " << cell << std::endl;
        }
    }


    void CopyCmdHelper(const std::string& from_path, const std::string& to_path, bool overwrite) {
        try {
            CHECK_NE(client::FileOrDie(from_path), "") << from_path << " is an empty dir or does not exist.";
            if (!overwrite && client::FileOrDie(to_path) != "") {
                LOG(FATAL) << "File already exists. Please use --f to overwrite.";
                return;
            }
            absl::StatusOr<std::string> to_path_or = galaxy::util::InitClient(to_path);
            absl::StatusOr<std::string> from_path_or = galaxy::util::InitClient(from_path);
            CHECK(from_path_or.ok() && to_path_or.ok()) << "Please make sure the paths are remote.";
            auto client = GetFileutilClient();
            DownloadRequest request;
            request.set_from_name(*from_path_or);
            request.set_to_name(to_path);
            request.mutable_cred()->set_password(absl::GetFlag(FLAGS_fs_password));
            client::RmFile(to_path);
            DownloadResponse response = client.CopyFile(request);
            FileSystemStatus status = response.status();
            CHECK_EQ(status.return_code(), 1) << "Fail to call Copy cmd.";
            std::cout << "Done copying from " << from_path << " to " << to_path << std::endl;
        }
        catch (std::string errorMsg)
        {
            LOG(ERROR) << errorMsg;
            throw errorMsg;
        }
    }

    void CopyCmd(const std::string& from_path, const std::string& to_path, bool overwrite) {
        try {
            std::map<std::string, std::string> sub_files = client::ListFilesInDirRecursive(from_path);
            // Needs to push back from_path in case from_path is a file name.
            std::vector<std::string> all_files;
            for (const auto& sub_file : sub_files) {
                all_files.push_back(sub_file.first);
            }
            if (all_files.empty()) {
                all_files.push_back(from_path);
            }
            std::vector<std::thread> threads;
            for (auto& file : all_files) {
                std::cout << file << std::endl;
                std::string new_file(file);
                new_file.replace(0, from_path.length(), to_path);
                threads.push_back(std::thread(CopyCmdHelper, file, new_file, overwrite));
            }

            for (auto& th : threads) {
                th.join();
            }
        }
        catch (std::string errorMsg)
        {
            LOG(FATAL) << errorMsg;
        }
    }

    void MoveCmd(const std::string& from_path, const std::string& to_path, bool overwrite) {
        try {
            CopyCmd(from_path, to_path, overwrite);
            client::RmDirRecursive(from_path);
            client::RmFile(from_path);
            std::cout << "Done moving from " << from_path << " to " << to_path << std::endl;
        }
        catch (std::string errorMsg)
        {
            LOG(FATAL) << errorMsg;
        }
    }
}
