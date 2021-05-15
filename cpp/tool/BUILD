load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

cc_library(
    name = "fileutil_lib",
    srcs = [
        "fileutil.h",
        "fileutil.cc",
    ],
    deps= [
        "//schema:fileserver_cc_grpc",
        "//cpp/core:galaxy_flag_lib",
        "//cpp/internal:galaxy_const_lib",
        "//cpp/util:galaxy_util_lib",
        "//cpp:client",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/flags:flag",
        "@google_glog//:glog",
        "@com_github_grpc_grpc//:grpc++",
    ]
)

cc_binary(
    name = "fileutil",
    srcs = [
        "fileutil_main.cc",
    ],
    visibility = ["//visibility:public"],
    deps= [
        ":fileutil_lib",
    ]
)
