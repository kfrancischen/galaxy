load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

cc_library(
    name = "fileutil_lib",
    srcs = [
        "fileutil.h",
        "fileutil.cc",
    ],
    deps= [
        "//cpp:client",
        "@com_google_absl//absl/flags:flag",
        "@google_glog//:glog",
    ],
    linkopts = ["-lpthread"],
)

cc_binary(
    name = "fileutil",
    srcs = [
        "fileutil_main.cc",
    ],
    deps= [
        ":fileutil_lib",
        "//cpp/core:galaxy_flag_lib",
        "//cpp/util:galaxy_util_lib",
    ]
)

cc_library(
    name = "pm2_cli_lib",
    srcs = [
        "pm2_cli.h",
        "pm2_cli.cc",
    ],
    deps= [
        "//cpp/core:galaxy_flag_lib",
        "//cpp:client",
        "@com_google_absl//absl/flags:flag",
        "@google_glog//:glog",
    ],
    linkopts = ["-lpthread"],
)

cc_binary(
    name = "pm2_cli",
    srcs = [
        "pm2_cli_main.cc",
    ],
    deps= [
        ":pm2_cli_lib",
    ]
)
