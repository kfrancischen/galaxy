#include <map>
#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "cpp/client.h"
#include "glog/logging.h"

namespace py = pybind11;

PYBIND11_MODULE(_gclient, m)
{
    google::InitGoogleLogging("GALAXY_CLIENT");
    m.doc() = "galaxy client"; // optional module docstring
    m.def("create_dir_if_not_exist", &galaxy::client::CreateDirIfNotExist, "Wrapper for CreateDirIfNotExist",
        py::arg("path"), py::arg("mode") = 0777);
    m.def("dir_or_die", &galaxy::client::DirOrDie, "Wrapper for DirOrDie", py::arg("path"));
    m.def("rm_dir", &galaxy::client::RmDir, "Wrapper for RmDir", py::arg("path"), py::arg("include_hidden")=false);
    m.def("rm_dir_recursive", &galaxy::client::RmDirRecursive, "Wrapper for RmDirRecursive", py::arg("path"), py::arg("include_hidden")=false);
    m.def("list_dirs_in_dir", &galaxy::client::ListDirsInDir, "Wrapper for ListDirsInDir", py::arg("path"));
    m.def("list_files_in_dir", &galaxy::client::ListFilesInDir, "Wrapper for ListFilesInDir", py::arg("path"), py::arg("include_hidden")=false);
    m.def("list_dirs_in_dir_recursive", &galaxy::client::ListDirsInDirRecursive, "Wrapper for ListDirsInDirRecursive", py::arg("path"));
    m.def("list_files_in_dir_recursive", &galaxy::client::ListFilesInDirRecursive, "Wrapper for ListFilesInDirRecursive", py::arg("path"), py::arg("include_hidden")=false);
    m.def("create_file_if_not_exist", &galaxy::client::CreateFileIfNotExist, "Wrapper for CreateFileIfNotExist",
        py::arg("path"), py::arg("mode") = 0777);
    m.def("file_or_die", &galaxy::client::FileOrDie, "Wrapper for FileOrDie", py::arg("path"));
    m.def("rm_file", &galaxy::client::RmFile, "Wrapper for RmFile", py::arg("path"), py::arg("is_hidden")=false);
    m.def("rename_file", &galaxy::client::RenameFile, "Wrapper for RenameFile", py::arg("old_path"), py::arg("new_path"));
    m.def("read", [](const std::string path) {
        std::string data = galaxy::client::Read(path);
        return py::bytes(data);
    },  "Wrapper for Read", py::arg("path"));
    m.def("read_multiple", [](const std::vector<std::string> paths) {
        std::map<std::string, std::string> data = galaxy::client::ReadMultiple(paths);
        std::map<std::string, py::bytes> result;
        for (const auto& val : data) {
            result.insert({val.first, py::bytes(val.second)});
        }
        return result;
    }, "Wrapper for ReadMultiple", py::arg("paths"));
    m.def("write", &galaxy::client::Write, "Wrapper for Write", py::arg("path"), py::arg("data"), py::arg("mode")="w");
    m.def("write_multiple", &galaxy::client::WriteMultiple, "Wrapper for WriteMultiple", py::arg("path_data_map"), py::arg("mode")="w");
    m.def("get_attr", &galaxy::client::GetAttr, "Wrapper for GetAttr", py::arg("path"));
    m.def("list_cells", &galaxy::client::ListCells, "Wrapper for ListCells");
    m.def("check_health", &galaxy::client::CheckHealth, "Wrapper for CheckHealth", py::arg("cell"));
    m.def("copy_file", &galaxy::client::CopyFile, "Wrapper for CopyFile", py::arg("from_path"), py::arg("to_path"));
}
