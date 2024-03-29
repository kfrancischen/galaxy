import os
import posixpath
import re
import shutil

from distutils import sysconfig
import setuptools
from setuptools.command import build_ext


class BazelExtension(setuptools.Extension):
    """A C/C++ extension that is defined as a Bazel BUILD target."""

    def __init__(self, name, bazel_target):
        self.bazel_target = bazel_target
        self.relpath, self.target_name = posixpath.relpath(bazel_target, "//").split(":")
        setuptools.Extension.__init__(self, name, sources=[])


class BuildBazelExtension(build_ext.build_ext):
    """A command that runs Bazel to build a C/C++ extension."""

    def run(self):
        for ext in self.extensions:
            self.bazel_build(ext)
        build_ext.build_ext.run(self)

    def bazel_build(self, ext):
        with open("WORKSPACE", "r") as f:
            workspace_contents = f.read()

        with open("WORKSPACE", "w") as f:
            f.write(
                re.sub(
                    r'(?<=path = ").*(?=",  # May be overwritten by setup\.py\.)',
                    sysconfig.get_python_inc().replace(os.path.sep, posixpath.sep),
                    workspace_contents,
                )
            )

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        bazel_argv = [
            "bazel",
            "build",
            ext.bazel_target,
            "--symlink_prefix=" + os.path.join(self.build_temp, "bazel-"),
            "--compilation_mode=" + ("dbg" if self.debug else "opt"),
        ]
        self.spawn(bazel_argv)

        if not ext.name.startswith("_"):
            ext.name = "_" + ext.name
        shared_lib_ext = ".so"
        shared_lib = ext.name + shared_lib_ext
        ext_bazel_bin_path = os.path.join(self.build_temp, "bazel-bin", ext.relpath, shared_lib)

        ext_dest_path = self.get_ext_fullpath(ext.name)
        ext_dest_dir = os.path.dirname(ext_dest_path)

        if not os.path.exists(ext_dest_dir):
            os.makedirs(ext_dest_dir)
        shutil.copyfile(ext_bazel_bin_path, ext_dest_path)

        package_dir = os.path.join(ext_dest_dir, "galaxy_py")
        if not os.path.exists(package_dir):
            os.makedirs(package_dir)

        shutil.copyfile(
            "python/__init__.py", os.path.join(package_dir, "__init__.py")
        )
        shutil.copyfile(
            "python/logging.py", os.path.join(package_dir, "logging.py")
        )


setuptools.setup(
    name="galaxy_py",
    version="1.0.0",
    description="Simple distributed file system based on gRPC.",
    long_description="README.md",
    keywords="distributed system, gRPC",
    url="https://github.com/kfrancischen/galaxy",
    python_requires=">=3.6",
    package_dir={"": "python"},
    cmdclass=dict(build_ext=BuildBazelExtension),
    packages=setuptools.find_packages(where="python"),
    include_package_data=True,
    ext_modules=[
        BazelExtension("gclient", "//python:gclient",)
    ],
    zip_safe=False,
    author="Francis Chen",
    author_email="kfrancischen@gmail.com",
    classifiers=[
        "Programming Language :: Python :: 3",
        "Operating System :: OS Independent",
        "Topic :: Software Development :: Libraries :: Python Modules",
        "Topic :: System :: Distributed Computing"
    ],
    install_requires=[],
    license="Apache 2.0",
)
