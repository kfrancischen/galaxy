load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "galaxy_const_lib",
    visibility = ["//cpp:__subpackages__"],
    srcs = [
        "galaxy_const.h",
    ],
)


cc_library(
    name = "galaxy_fs_internal_lib",
    visibility = ["//cpp/core:__subpackages__"],
    srcs = [
        "galaxy_fs_internal.h",
        "galaxy_fs_internal.cc",
    ],
    deps= [
        ":galaxy_const_lib",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/status:status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/container:flat_hash_map",
        "@google_glog//:glog"
    ]
)

cc_library(
    name = "galaxy_client_internal_lib",
    visibility = ["//cpp:__subpackages__"],
    srcs = [
        "galaxy_client_internal.h",
        "galaxy_client_internal.cc",
    ],
    deps= [
        "//schema:fileserver_cc_grpc",
        "//cpp/core:galaxy_flag_lib",
        "//cpp/internal:galaxy_const_lib",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/flags:flag",
        "@google_glog//:glog",
        "@com_github_grpc_grpc//:grpc++",
    ]
)

cc_library(
    name = "galaxy_stats_internal_lib",
    visibility = ["//cpp:__subpackages__"],
    srcs = [
        "galaxy_stats_internal.h",
        "galaxy_stats_internal.cc",
    ],
    deps= [
        "@com_github_jupp0r_prometheus_cpp//pull",
        "@io_opencensus_cpp//opencensus/stats",
        "@io_opencensus_cpp//opencensus/tags",
        "@com_google_absl//absl/strings",
    ]
)
