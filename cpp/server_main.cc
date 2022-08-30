#include <iostream>
#include <string>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/ext/channelz_service_plugin.h>
#include <grpcpp/opencensus.h>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "cpp/core/galaxy_server.h"
#include "cpp/core/galaxy_flag.h"
#include "cpp/core/galaxy_stats.h"
#include "cpp/util/galaxy_util.h"
#include "glog/logging.h"
#include "opencensus/exporters/stats/prometheus/prometheus_exporter.h"
#include "prometheus/exposer.h"
#include "schema/fileserver.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using galaxy::GalaxyServerImpl;

void RunGalaxyServer(const galaxy_schema::CellConfig& config)
{
    std::string stats_address("0.0.0.0:" + std::to_string(config.fs_stats_port()));
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
        [&config](const std::string& user, const std::string& password)
        {
            return user == "admin" && password == config.fs_password();
        });
    exposer.RegisterCollectable(exporter);

    // Init custom measure.
    galaxy::stats::RegisterServerViews();
    LOG(INFO) << "Stats are exposed to " << stats_address << ".";

    std::string server_address("0.0.0.0:" + std::to_string(config.fs_port()));
    GalaxyServerImpl galaxy_service;
    galaxy_service.SetPassword(config.fs_password());

    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::channelz::experimental::InitChannelzService();
    ServerBuilder builder;
    if (config.fs_num_thread() > 0) {
        grpc::ResourceQuota rq;
        rq.SetMaxThreads(config.fs_num_thread());
        builder.SetResourceQuota(rq);
    }
    builder.SetMaxMessageSize(config.fs_max_msg_size() * 1024 * 1024);
    builder.AddChannelArgument(GRPC_ARG_ENABLE_CHANNELZ, 1);

    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&galaxy_service);

    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    LOG(INFO) << "Server listening on " << server_address << ".";
    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
}

int main(int argc, char* argv[])
{
    absl::ParseCommandLine(argc, argv);
    FLAGS_colorlogtostderr = true;
    absl::StatusOr<galaxy_schema::CellConfig> config = galaxy::util::ParseCellConfig(absl::GetFlag(FLAGS_fs_cell));
    CHECK(config.ok()) << "Fail to parse the cell config.";
    FLAGS_v = config->fs_verbose_level();
    FLAGS_log_dir = config->fs_log_dir();
    FLAGS_max_log_size = 10;  // setting the maximum log size to 10M
    FLAGS_alsologtostderr = config->fs_alsologtostderr();
    google::EnableLogCleaner(config->fs_log_ttl());
    google::InitGoogleLogging(argv[0]);
    LOG(INFO) << "Getting config for cell: " << absl::GetFlag(FLAGS_fs_cell);
    LOG(INFO) << std::endl << config->DebugString();
    RunGalaxyServer(*config);
    return 0;
}