load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

package(default_visibility = ["//visibility:public"])

proto_library(
    name = "fileserver_proto",
    srcs = ["fileserver.proto"],
)

cc_proto_library(
    name = "fileserver_cc_proto",
    deps = [":fileserver_proto"]
)

cc_grpc_library(
    name = "fileserver_cc_grpc_proto",
    srcs = [":fileserver_proto"],
    grpc_only = True,
    deps = [":fileserver_cc_proto"],
)
