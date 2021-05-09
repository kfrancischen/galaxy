load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = 'galaxy_fs_example',
    srcs = [
        'galaxy_fs_example.cc',
    ],
    deps = [
        "//cpp/core:galaxy_fs_lib",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:flag",
        "@google_glog//:glog"
    ]
)

cc_binary(
    name = 'client_example',
    srcs = [
        'client_example.cc',
    ],
    deps = [
        "//cpp:client",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:flag",
    ]
)
