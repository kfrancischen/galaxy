load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "galaxy_server",
    srcs = [
        'server_main.cc',
    ],
    defines = ["BAZEL_BUILD"],
    deps = [
        "//cpp/core:galaxy_server_impl_lib",
        "//cpp/core:galaxy_flag_lib",
        "//cpp/util:galaxy_util_lib",
        "//cpp/core:galaxy_stats_lib",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@google_glog//:glog",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "@com_github_grpc_grpc//:grpcpp_channelz",
        "@com_github_grpc_grpc//:grpc_opencensus_plugin",
        "@com_github_jupp0r_prometheus_cpp//pull",
        "@io_opencensus_cpp//opencensus/exporters/stats/prometheus:prometheus_exporter",
    ]
)

cc_library(
    name = "client",
    srcs = [
        "client.h",
        "client.cc",
    ],
    visibility = ["//visibility:public"],
    deps= [
        "//schema:fileserver_cc_grpc",
        "//cpp/core:galaxy_flag_lib",
        "//cpp/internal:galaxy_client_internal_lib",
        "//cpp/internal:galaxy_const_lib",
        "//cpp/util:galaxy_util_lib",
        "//cpp/core:galaxy_fs_lib",
        "@google_glog//:glog",
        "@com_google_absl//absl/flags:flag",
        "@com_github_grpc_grpc//:grpc++",
        "@rapidjson",
    ]
)
