#include <iostream>
#include <fstream>
#include <string>
#include "absl/flags/flag.h"
#include "cpp/remote_execution.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/tool/pm2_cli.h"
#include "cpp/client.h"
#include "cpp/util/galaxy_util.h"
#include "glog/logging.h"

using grpc::ClientContext;
using grpc::Status;
using galaxy_schema::CellConfig;

using galaxy_schema::RemoteExecutionRequest;
using galaxy_schema::RemoteExecutionResponse;
using galaxy_schema::FileSystemStatus;

namespace galaxy
{

    void Pm2List(const std::string &cell)
    {
        try
        {
            absl::StatusOr<CellConfig> config = util::ParseCellConfig(cell);
            auto client = remote_execution::GetRemoteExeClient(*config);
            RemoteExecutionRequest request;
            request.mutable_cred()->set_password(config->fs_password());
            request.set_from_cell(config->cell());
            request.set_main("pm2");
            *request.add_program_args() = "list";
            RemoteExecutionResponse response = client.RemoteExecution(request);
            FileSystemStatus status = response.status();
            CHECK_EQ(status.return_code(), 1) << "Fail to call Pm2List cmd.";
            std::cout << "Done executing cmd [" << response.raw_cmd() << "]"
                      << " on cell [" << cell << "] with output:" << std::endl;
            std::cout << response.data() << std::endl;
        }
        catch (std::string errorMsg)
        {
            LOG(FATAL) << errorMsg;
        }
    }

    void Pm2List()
    {
        std::vector<std::string> cells = client::ListCells();
        for (const auto& cell : cells) {
            std::cout << "Checking cell: " << cell << std::endl;
            Pm2List(cell);
        }
    }

    void Pm2Start(const std::string &cell, const std::string &home_dir, const std::string &json_file, const std::string &job_name)
    {
        try
        {
            absl::StatusOr<CellConfig> config = util::ParseCellConfig(cell);
            auto client = remote_execution::GetRemoteExeClient(*config);
            RemoteExecutionRequest request;
            request.mutable_cred()->set_password(config->fs_password());
            request.set_from_cell(config->cell());
            request.set_home_dir(home_dir);
            request.set_main("pm2");
            *request.add_program_args() = "start";
            *request.add_program_args() = json_file;
            *request.add_program_args() = "--only " + job_name;
            RemoteExecutionResponse response = client.RemoteExecution(request);
            FileSystemStatus status = response.status();
            CHECK_EQ(status.return_code(), 1) << "Fail to call Pm2Start cmd.";
            std::cout << "Done executing cmd [" << response.raw_cmd() << "]"
                      << " on cell [" << cell << "] with output:" << std::endl;
            std::cout << response.data() << std::endl;
        }
        catch (std::string errorMsg)
        {
            LOG(FATAL) << errorMsg;
        }
    }

    void Pm2Stop(const std::string &cell, const std::string &job_name)
    {
        try
        {
            absl::StatusOr<CellConfig> config = util::ParseCellConfig(cell);
            auto client = remote_execution::GetRemoteExeClient(*config);
            RemoteExecutionRequest request;
            request.mutable_cred()->set_password(config->fs_password());
            request.set_from_cell(config->cell());
            request.set_main("pm2");
            *request.add_program_args() = "stop";
            *request.add_program_args() = job_name;
            RemoteExecutionResponse response = client.RemoteExecution(request);
            FileSystemStatus status = response.status();
            CHECK_EQ(status.return_code(), 1) << "Fail to call Pm2Stop cmd.";
            std::cout << "Done executing cmd [" << response.raw_cmd() << "]"
                      << " on cell [" << cell << "] with output:" << std::endl;
            std::cout << response.data() << std::endl;
        }
        catch (std::string errorMsg)
        {
            LOG(FATAL) << errorMsg;
        }
    }

    void Pm2Stop(const std::string &job_name) {
        std::vector<std::string> cells = client::ListCells();
        for (const auto& cell : cells) {
            std::cout << "Checking cell: " << cell << std::endl;
            Pm2Stop(cell, job_name);
        }
    }

    void Pm2Restart(const std::string &cell, const std::string &job_name)
    {
        try
        {
            absl::StatusOr<CellConfig> config = util::ParseCellConfig(cell);
            auto client = remote_execution::GetRemoteExeClient(*config);
            RemoteExecutionRequest request;
            request.mutable_cred()->set_password(config->fs_password());
            request.set_from_cell(config->cell());
            request.set_main("pm2");
            *request.add_program_args() = "restart";
            *request.add_program_args() = job_name;
            RemoteExecutionResponse response = client.RemoteExecution(request);
            FileSystemStatus status = response.status();
            CHECK_EQ(status.return_code(), 1) << "Fail to call Pm2Restart cmd.";
            std::cout << "Done executing cmd [" << response.raw_cmd() << "]"
                      << " on cell [" << cell << "] with output:" << std::endl;
            std::cout << response.data() << std::endl;
        }
        catch (std::string errorMsg)
        {
            LOG(FATAL) << errorMsg;
        }
    }

    void Pm2Restart(const std::string &job_name) {
        std::vector<std::string> cells = client::ListCells();
        for (const auto& cell : cells) {
            std::cout << "Checking cell: [" << cell << "]" << std::endl;
            Pm2Restart(cell, job_name);
        }
    }
}
