

#include <iostream>
#include <string>

#include "cpp/core/galaxy_server.h"
#include "cpp/core/galaxy_flag.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using galaxy::GalaxyServerImpl;

void RunGalaxyServer()
{
    std::string server_address(absl::GetFlag(FLAGS_fs_address));
    GalaxyServerImpl galaxy_service;
    galaxy_service.SetPassword(absl::GetFlag(FLAGS_fs_password));

    ServerBuilder builder;

    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&galaxy_service);

    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char* argv[])
{
    absl::ParseCommandLine(argc, argv);
    RunGalaxyServer();
    return 0;
}