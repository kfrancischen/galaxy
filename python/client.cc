#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "cpp/client.h"

namespace py = pybind11;

PYBIND11_MODULE(_gclient, m) {
    m.doc() = "pybind11 galaxy client"; // optional module docstring
    m.def("create_dir_if_not_exist", &galaxy::client::CreateDirIfNotExist, "Wrapper for CreateDirIfNotExist",
            py::arg("path"), py::arg("mode")=0777);
    m.def("dir_or_die", &galaxy::client::DirOrDie, "Wrapper for DirOrDie", py::arg("path"));
    m.def("rm_dir", &galaxy::client::RmDir, "Wrapper for RmDir", py::arg("path"));
    m.def("rm_dir_recursive", &galaxy::client::RmDirRecursive, "Wrapper for RmDirRecursive", py::arg("path"));
    m.def("list_dirs_in_dir", &galaxy::client::ListDirsInDir, "Wrapper for ListDirsInDir", py::arg("path"));
    m.def("list_files_in_dir", &galaxy::client::ListFilesInDir, "Wrapper for ListFilesInDir", py::arg("path"));
    m.def("create_file_if_not_exist", &galaxy::client::CreateFileIfNotExist, "Wrapper for CreateFileIfNotExist", py::arg("path"), py::arg("mode")=0777);
    m.def("file_or_die", &galaxy::client::FileOrDie, "Wrapper for FileOrDie", py::arg("path"));
    m.def("rm_file", &galaxy::client::RmFile, "Wrapper for RmFile", py::arg("path"));
    m.def("rename_file", &galaxy::client::RenameFile, "Wrapper for RenameFile", py::arg("old_path"), py::arg("new_path"));
    m.def("read", &galaxy::client::Read, "Wrapper for Read", py::arg("path"));
    m.def("write", &galaxy::client::Write, "Wrapper for Write", py::arg("path"), py::arg("data"), py::arg("mode")="w");
    m.def("get_attr", &galaxy::client::GetAttr, "Wrapper for GetAttr", py::arg("path"));
}
