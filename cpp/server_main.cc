#include <iostream>
#include <string>

#include "cpp/core/galaxy_server.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/core/galaxy_stats.h"
#include "cpp/util/galaxy_util.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/ext/channelz_service_plugin.h>
#include <grpcpp/opencensus.h>
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "glog/logging.h"
#include "opencensus/exporters/stats/prometheus/prometheus_exporter.h"
#include "prometheus/exposer.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using galaxy::GalaxyServerImpl;

void RunGalaxyServer()
{
    std::string stats_address(absl::GetFlag(FLAGS_fs_stats_address));
    // Register the OpenCensus gRPC plugin to enable stats and tracing in gRPC.
    grpc::RegisterOpenCensusPlugin();
    // Register the gRPC views (latency, error count, etc).
    grpc::RegisterOpenCensusViewsForExport();
    // Keep a shared pointer to the Prometheus exporter.
    auto exporter =
        std::make_shared<opencensus::exporters::stats::PrometheusExporter>();
    // Expose a Prometheus endpoint.
    prometheus::Exposer exposer(stats_address);
    exposer.RegisterAuth(
        [](const std::string &user, const std::string &password)
        {
            return user == "admin" && password == absl::GetFlag(FLAGS_fs_password);
        });
    exposer.RegisterCollectable(exporter);

    // Init custom measure.
    galaxy::stats::RegisterServerViews();
    std::cout << "Stats are exposed to " << stats_address << std::endl;
    LOG(INFO) << "Stats are exposed to " << stats_address << ".";

    std::string server_address(absl::GetFlag(FLAGS_fs_address));
    GalaxyServerImpl galaxy_service;
    galaxy_service.SetPassword(absl::GetFlag(FLAGS_fs_password));

    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::channelz::experimental::InitChannelzService();
    ServerBuilder builder;
    grpc::ResourceQuota rq;
    rq.SetMaxThreads(absl::GetFlag(FLAGS_fs_num_thread));
    builder.SetResourceQuota(rq);
    builder.SetMaxMessageSize(absl::GetFlag(FLAGS_fs_max_msg_size) * 1024 * 1024);
    builder.AddChannelArgument(GRPC_ARG_ENABLE_CHANNELZ, 1);

    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&galaxy_service);

    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    LOG(INFO) << "Server listening on " << server_address << ".";
    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char* argv[])
{
    absl::ParseCommandLine(argc, argv);
    FLAGS_colorlogtostderr = true;
    absl::StatusOr<std::string> result = galaxy::util::ParseGlobalConfig(true);
    CHECK(result.ok()) << "Fail to parse the global config.";
    FLAGS_v = absl::GetFlag(FLAGS_fs_verbose_level);
    FLAGS_log_dir = absl::GetFlag(FLAGS_fs_log_dir);
    FLAGS_alsologtostderr = absl::GetFlag(FLAGS_fs_alsologtostderr);
    google::EnableLogCleaner(absl::GetFlag(FLAGS_fs_log_ttl));
    google::InitGoogleLogging(argv[0]);
    LOG(INFO) << *result;
    RunGalaxyServer();
    return 0;
}