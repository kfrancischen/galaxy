#ifndef CPP_REMOTE_EXECUTION_H
#define CPP_REMOTE_EXECUTION_H

#include <grpcpp/grpcpp.h>
#include "schema/fileserver.grpc.pb.h"

namespace galaxy
{
    namespace remote_execution
    {
        class GalaxyRemoteExe
        {
        public:
            GalaxyRemoteExe(std::shared_ptr<grpc::Channel> channel) : stub_(galaxy_schema::FileSystem::NewStub(channel)) {}
            galaxy_schema::RemoteExecutionResponse RemoteExecution(const galaxy_schema::RemoteExecutionRequest &request);

        private:
            std::unique_ptr<galaxy_schema::FileSystem::Stub> stub_;
        };

        GalaxyRemoteExe GetRemoteExeClient(const galaxy_schema::CellConfig& config);
    } // namespace remote_execution
} // namespace galaxy

#endif  // CPP_REMOTE_EXECUTION_H
