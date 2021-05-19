load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "galaxy_flag_lib",
    srcs = [
        "galaxy_flag.h",
        "galaxy_flag.cc",
    ],
    deps = [
        "@com_google_absl//absl/flags:flag",
    ]
)

cc_library(
    name = "galaxy_fs_lib",
    srcs = [
        "galaxy_fs.h",
        "galaxy_fs.cc",
    ],
    deps= [
        ":galaxy_flag_lib",
        "//cpp/internal:galaxy_fs_internal_lib",
        "@google_glog//:glog"
    ]
)

cc_library(
    name = "galaxy_server_impl_lib",
    srcs = [
        "galaxy_server.h",
        "galaxy_server.cc",
    ],
    deps= [
        ":galaxy_fs_lib",
        "//schema:fileserver_cc_grpc",
        "//cpp/internal:galaxy_const_lib",
        "//cpp/internal:galaxy_stats_internal_lib",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/time:time",
    ]
)

cc_library(
    name = "galaxy_stats_lib",
    srcs = [
        "galaxy_stats.h",
        "galaxy_stats.cc",
    ],
    deps = [
        "//cpp/internal:galaxy_stats_internal_lib",
        "@google_glog//:glog"
    ]
)