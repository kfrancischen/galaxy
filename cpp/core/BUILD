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
    visibility = ["//visibility:public"],
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
    visibility = ["//visibility:public"],
    srcs = [
        "galaxy_server.h",
        "galaxy_server.cc",
    ],
    deps= [
        "//schema:fileserver_cc_grpc_proto",
        "@com_github_grpc_grpc//:grpc++",
    ]
)

cc_binary(
    name = "galaxy_server",
    srcs = [
        'galaxy_server_main.cc',
    ],
    defines = ["BAZEL_BUILD"],
    deps = [
        ":galaxy_server_impl_lib",
        ":galaxy_flag_lib",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
    ]
)