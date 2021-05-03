cc_library(
    name = "galaxy_flag_lib",
    srcs = [
        "galaxy_flag.h",
        "galaxy_flag.cc",
    ],
    deps = [
        "@google_absl//absl/flags:flag",
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