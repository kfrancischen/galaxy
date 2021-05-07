load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "galaxy_server",
    srcs = [
        'server_main.cc',
    ],
    defines = ["BAZEL_BUILD"],
    deps = [
        "//cpp/core:galaxy_server_impl_lib",
        "//cpp/core:galaxy_flag_lib",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@google_glog//:glog"
    ]
)