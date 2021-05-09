#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "cpp/client.h"

namespace py = pybind11;

PYBIND11_MODULE(_gclient, m)
{
    m.doc() = "galaxy client"; // optional module docstring
    m.def("create_dir_if_not_exist", &galaxy::client::CreateDirIfNotExist, "Wrapper for CreateDirIfNotExist",
          py::arg("path"), py::arg("mode") = 0777);
    m.def("dir_or_die", &galaxy::client::DirOrDie, "Wrapper for DirOrDie", py::arg("path"));
    m.def("rm_dir", &galaxy::client::RmDir, "Wrapper for RmDir", py::arg("path"));
    m.def("rm_dir_recursive", &galaxy::client::RmDirRecursive, "Wrapper for RmDirRecursive", py::arg("path"));
    m.def("list_dirs_in_dir", &galaxy::client::ListDirsInDir, "Wrapper for ListDirsInDir", py::arg("path"));
    m.def("list_files_in_dir", &galaxy::client::ListFilesInDir, "Wrapper for ListFilesInDir", py::arg("path"));
    m.def("create_file_if_not_exist", &galaxy::client::CreateFileIfNotExist, "Wrapper for CreateFileIfNotExist", py::arg("path"), py::arg("mode") = 0777);
    m.def("file_or_die", &galaxy::client::FileOrDie, "Wrapper for FileOrDie", py::arg("path"));
    m.def("rm_file", &galaxy::client::RmFile, "Wrapper for RmFile", py::arg("path"));
    m.def("rename_file", &galaxy::client::RenameFile, "Wrapper for RenameFile", py::arg("old_path"), py::arg("new_path"));
    m.def("read", &galaxy::client::Read, "Wrapper for Read", py::arg("path"));
    m.def("write", &galaxy::client::Write, "Wrapper for Write", py::arg("path"), py::arg("data"), py::arg("mode") = "w");
    m.def("get_attr", &galaxy::client::GetAttr, "Wrapper for GetAttr", py::arg("path"));

    m.def("lcreate_dir_if_not_exist", &galaxy::client::LCreateDirIfNotExist, "Wrapper for local CreateDirIfNotExist",
          py::arg("path"), py::arg("mode") = 0777);
    m.def("ldir_or_die", &galaxy::client::LDirOrDie, "Wrapper for local DirOrDie", py::arg("path"));
    m.def("lrm_dir", &galaxy::client::LRmDir, "Wrapper for local RmDir", py::arg("path"));
    m.def("lrm_dir_recursive", &galaxy::client::LRmDirRecursive, "Wrapper for local RmDirRecursive", py::arg("path"));
    m.def("llist_dirs_in_dir", &galaxy::client::LListDirsInDir, "Wrapper for local ListDirsInDir", py::arg("path"));
    m.def("llist_files_in_dir", &galaxy::client::LListFilesInDir, "Wrapper for local ListFilesInDir", py::arg("path"));
    m.def("lcreate_file_if_not_exist", &galaxy::client::LCreateFileIfNotExist, "Wrapper for local CreateFileIfNotExist", py::arg("path"), py::arg("mode") = 0777);
    m.def("lfile_or_die", &galaxy::client::LFileOrDie, "Wrapper for local FileOrDie", py::arg("path"));
    m.def("lrm_file", &galaxy::client::LRmFile, "Wrapper for local RmFile", py::arg("path"));
    m.def("lrename_file", &galaxy::client::LRenameFile, "Wrapper for local RenameFile", py::arg("old_path"), py::arg("new_path"));
    m.def("lread", &galaxy::client::LRead, "Wrapper for local Read", py::arg("path"));
    m.def("lwrite", &galaxy::client::LWrite, "Wrapper for local Write", py::arg("path"), py::arg("data"), py::arg("mode") = "w");
}
