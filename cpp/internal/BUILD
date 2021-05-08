load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "galaxy_fs_internal_lib",
    visibility = ["//cpp/core:__subpackages__"],
    srcs = [
        "galaxy_fs_internal.h",
        "galaxy_fs_internal.cc",
    ],
    deps= [
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/status:status",
        "@com_google_absl//absl/status:statusor",
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
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@google_glog//:glog",
        "@com_github_grpc_grpc//:grpc++",
    ]
)
