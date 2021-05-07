load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")
load("@com_github_grpc_grpc//bazel:python_rules.bzl", "py_proto_library", "py_grpc_library")

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
    name = "fileserver_cc_grpc",
    srcs = [":fileserver_proto"],
    grpc_only = True,
    deps = [":fileserver_cc_proto"],
)

py_proto_library(
    name = "fileserver_py_pb2",
    deps = [":fileserver_proto"],
)

py_grpc_library(
    name = "fileserver_py_pb2_grpc",
    deps = [":fileserver_py_pb2"],
    srcs = [":fileserver_proto"],
)
