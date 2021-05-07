#ifndef CPP_GALAXY_CLIENT_H
#define CPP_GALAXY_CLIENT_H

#include <grpcpp/grpcpp.h>
#include "schema/fileserver.grpc.pb.h"

namespace galaxy
{
    class GalaxyClient
    {
    public:
        GalaxyClient(std::shared_ptr<grpc::Channel> channel) : stub_(galaxy_schema::FileSystem::NewStub(channel)) {}
        

    private:
        std::unique_ptr<galaxy_schema::FileSystem::Stub> stub_;
    };

} // namespace galaxy

#endif // CPP_GALAXY_CLIENT_H