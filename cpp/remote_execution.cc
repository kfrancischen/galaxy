#include <iostream>
#include <fstream>
#include <string>
#include "cpp/remote_execution.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/util/galaxy_util.h"
#include "absl/flags/flag.h"
#include "glog/logging.h"

using grpc::ClientContext;
using grpc::Status;

using galaxy_schema::RemoteExecutionRequest;
using galaxy_schema::RemoteExecutionResponse;

using galaxy_schema::CellConfig;

namespace galaxy
{
    RemoteExecutionResponse remote_execution::GalaxyRemoteExe::RemoteExecution(const RemoteExecutionRequest &request)
    {
        RemoteExecutionResponse reply;
        ClientContext context;
        context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(absl::GetFlag(FLAGS_fs_rpc_ddl)));
        Status status = stub_->RemoteExecution(&context, request, &reply);
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

    remote_execution::GalaxyRemoteExe remote_execution::GetRemoteExeClient(const CellConfig& config)
    {
        grpc::ChannelArguments ch_args;
        ch_args.SetMaxReceiveMessageSize(-1);
        remote_execution::GalaxyRemoteExe client(grpc::CreateCustomChannel(config.fs_ip() + ":" + std::to_string(config.fs_port()), grpc::InsecureChannelCredentials(), ch_args));
        return client;
    }
}
