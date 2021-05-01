load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_proto_grpc//cpp:defs.bzl", "cpp_proto_library")
load("@rules_proto_grpc//cpp:defs.bzl", "cpp_grpc_library")

proto_library(
    name = "fileserver_proto",
    srcs = ["fileserver.proto"],
)

cpp_proto_library(
    name = "fileserver_cc_proto",
    protos = ["fileserver_proto"]
)

cpp_grpc_library(
    name = "fileserver_cc_grpc_proto",
    protos = ["fileserver_proto"],
)
