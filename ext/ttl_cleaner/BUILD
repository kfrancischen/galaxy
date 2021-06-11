load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

cc_library(
    name = "ttl_cleaner_lib",
    srcs = [
        "galaxy_ttl_cleaner.h",
        "galaxy_ttl_cleaner.cc",
    ],
    deps= [
        "//cpp/core:galaxy_fs_lib",
    ]
)

cc_binary(
    name = "galaxy_ttl_cleaner",
    srcs = [
        'galaxy_ttl_cleaner_main.cc',
    ],
    deps = [
        ":ttl_cleaner_lib",
        "//cpp/util:galaxy_util_lib",
        "//cpp/core:galaxy_flag_lib",
        "//cpp:client",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@google_glog//:glog",
    ]
)