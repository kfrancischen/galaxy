
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")
load("@rules_python//python:defs.bzl", "py_library")

package(default_visibility = ["//visibility:public"])

pybind_extension(
    name = "_gclient",
    srcs = ["client.cc"],
    deps = [
        "//cpp:client",
        "//cpp/util:galaxy_util_lib",
        "@google_glog//:glog",
    ],
    linkstatic = True,
)

py_library(
    name = "gclient",
    srcs = [
        "__init__.py",
    ],
    data = [
        ":_gclient.so",
    ],
    srcs_version = "PY3",
)
