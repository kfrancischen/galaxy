load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "galaxy_util_lib",
    visibility = ["//cpp:__subpackages__", "//ext:__subpackages__"],
    srcs = [
        "galaxy_util.h",
        "galaxy_util.cc",
    ],
    deps= [
        "@rapidjson",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/status:status",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings",
        "@google_glog//:glog",
        "//cpp/core:galaxy_flag_lib",
        "//cpp/internal:galaxy_const_lib",
    ]
)
