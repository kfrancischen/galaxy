# galaxy

Simple distributed file system based on gRPC.

[![996.icu](https://img.shields.io/badge/link-996.icu-red.svg)](https://996.icu)

This is a toy-version of distributed file system developed based on gRPC, and it will be integrated with [pslx](https://github.com/kfrancischen/pslx) shortly. The core logic is developed with C++ and later wrapped with Python using [Pybind11](https://github.com/pybind/pybind11). The build system for the whole package is [bazel](https://bazel.build/). In addition, several public versions of Google internal tools are used: [abseil](https://abseil.io/docs/cpp/quickstart), [glog](https://github.com/google/glog). [rapidjson](https://rapidjson.org/) is used to parse the config files. [prometheus](https://prometheus.io/) and [opencensus](https://opencensus.io/) are integrated to monitor the server side rpc.

## Core concepts
- `cell`: a cell is a machine that can be added as part of the filesystem, and is associated with a cell name. In galaxy, we use a two letter name for a cell, i.e. `aa`. The path for a cell in galaxy filesystem starts with `/galaxy/${CELL}-d/...`, where the `${CELL}` is the name of the cell. To make a machine as a cell in the filesystem, one just needs to launch the server code on the machine. Details are discussed in the next section.
- `global config`: a configuration file containing configurations for each cell in the galaxy filesystem. An example of it is in [server_config_example.json](https://github.com/kfrancischen/galaxy/blob/master/example/cpp/server_config_example.json).

## Server Entry Points
The entry point for galaxy filesystem server is located at [server_main.cc](https://github.com/kfrancischen/galaxy/blob/master/cpp/server_main.cc). To run the binary, one can use the following cmd
```shellscript
bazel run //cpp:galaxy_server -- \
--fs_global_config=/home/pslx/galaxy/example/cpp/server_config_example.json \
--fs_cell=aa
```
With the above cmd, the machine is added as cell `aa` with configurations specified in the `server_config_example.json` file.

## Client Python API
galaxy provides unified API for client to access both local and remote files, to build the python modules, please following the cmd of
```shellscript
python setup.py install
```

The modules: `gclient` and `gclient_ext` will be built as part of `galaxy_py`. The following functions are provided under `gclient` module
```python
create_dir_if_not_exist(path, mode=0777)
```
* Description: create a directory (along with its parent directories) if it does not exist.
* Args:
    1. path: the path to the directory
    2. mode: the permission mode

```python
dir_or_die(path)
```
* Decription: get the path to the directory if exist, other wise empty.
* Args:
    1. path: the path to the directory

```python
rm_dir(path)
```
* Decription: remove a directory and the files in the directory.
* Args:
    1. path: the path to the directory

```python
rm_dir_recursive(path)
```
* Decription: remove an empty directory recursively (including all the children files and directories).
* Args:
    1. path: the path to the directory

```python
list_dirs_in_dir(path)
```
* Decription: list all directories in a directory.
* Args:
    1. path: the path to the directory

```python
list_files_in_dir(path)
```
* Decription: list all files in a directory.
* Args:
    1. path: the path to the directory

```python
list_dirs_in_dir_recursive(path)
```
* Decription: list all directories in a directory and its subdirectories.
* Args:
    1. path: the path to the directory

```python
list_files_in_dir_recursive(path)
```
* Decription: list all files in a directory and its subdirectories.
* Args:
    1. path: the path to the directory

```python
create_file_if_not_exist(path, mode=0777)
```
* Description: create a file (along with its parent directories) if it does not exist.
* Args:
    1. path: the path to the file
    2. mode: the permission mode

```python
file_or_die(path)
```
* Decription: get the path to the file if exist, other wise empty.
* Args:
    1. path: the path to the direcfiletory

```python
rm_file(path)
```
* Decription: remove a file.
* Args:
    1. path: the path to the file

```python
rename_file(old_path, new_path)
```
* Decription: rename a file on the same cell
* Args:
    1. old_path: the path to the old file
    2. new_path: the path to the new file

```python
read(path)
```
* Decription: read a file (Note: the return is in the form of raw bytes).
* Args:
    1. path: the path to the file

```python
read_multiple(paths)
```
* Decription: read a list of files (Note: the return is in the form of raw bytes).
* Args:
    1. paths: the paths to the files


```python
write(path, data, mode="w")
```
* Decription: write data to a file.
* Args:
    1. path: the path to the file
    2. data: the data in string format
    3. mode: `w` means overwrite and `a` means append.

```python
get_attr(path)
```
* Decription: get attribute information of a file or a directory.
* Args:
    1. path: the path to the file or directory

In addition, in `gclient_ext` module, a few extension functions are provided

```python
write_proto_message(path, data, mode="w")
```
* Decription: write a protobuf data to a file.
* Args:
    1. path: the path to the file
    2. data: the data in protobuf message format
    3. mode: "w" means overwrite and "a" means append.

```python
read_proto_message(path, message_type)
```
* Decription: read a protobuf message file.
* Args:
    1. path: the path to the protobuf message file
    2. message_type: the protobuf type of the message


```python
read_txt(path)
```
* Decription: read a text file.
* Args:
    1. path: the path to the text file

```python
read_proto_messages(paths, message_type)
```
* Decription: read a list of protobuf message files.
* Args:
    1. paths: the paths to the protobuf message files
    2. message_type: the protobuf type of the message

```python
read_txts(paths)
```
* Decription: read a list of text files.
* Args:
    1. paths: the paths to the text files

```python
list_all_in_dir(path)
```
* Decription: list all directories and files in a directory.
* Args:
    1. path: the path to the directory

```python
list_all_in_dir_recursive(path)
```
* Decription: list all directories and files in a directory and subdirectories.
* Args:
    1. path: the path to the directory

```python
cp_file(from_path, to_path)
```
* Decription: copy a file from from_path to to_path. Note these two paths could be in the same cell or different cells.
* Args:
    1. from_path: the path to the file
    2. to_path: the path to the copied file

```python
mv_file(from_path, to_path)
```
* Decription: move a file from from_path to to_path. Note these two paths could be in the same cell or different cells.
* Args:
    1. from_path: the path to the file
    2. to_path: the path to the moved file

```python
cp_folder(from_path, to_path)
```
* Decription: copy a folder from from_path to to_path. Note these two paths could be in the same cell or different cells.
* Args:
    1. from_path: the path to the folder
    2. to_path: the path to the copied folder

```python
mv_folder(from_path, to_path)
```
* Decription: move a folder from from_path to to_path. Note these two paths could be in the same cell or different cells.
* Args:
    1. from_path: the path to the folder
    2. to_path: the path to the moved folder

## Fileutil tool
fileutil is an entry point for file operations across different cells. The entry point is located at [fileutil_main.cc](https://github.com/kfrancischen/galaxy/blob/master/cpp/tool/fileutil_main.cc). To build the binary, the bazel cmd is
```shellscript
bazel build -c opt //cpp/tool:fileutil
```

The following cmds are supported:

```shellscript
fileutil ls ${DIR_NAME}
```
* Description: list all the contents in the remote directory.

```shellscript
fileutil get ${REMOTE_FILE} ${LOCAL_FILE}
```
* Description: download remote file to local file.

```shellscript
fileutil upload ${LOCAL_FILE} ${REMOTE_FILE}
```
* Description: upload local file to remote file.

```shellscript
fileutil cp ${REMOTE_FILE1/REMOTE_DIR1} ${REMOTE_FILE2/REMOTE_DIR2} [--f]
```
* Description: copy one remote file/directory to another remote file/directory. They can be at different cells. Overwrite if `--f` is set.

```shellscript
fileutil mv ${REMOTE_FILE1/REMOTE_DIR1} ${REMOTE_FILE2/REMOTE_DIR2} [--f]
```
* Description: move one remote file/directory to another remote file/directory. They can be at different cells. Overwrite if `--f` is set.

```shellscript
fileutil rm ${REMOTE_DIR/REMOTE_FILE} [--r]
```
* Description: delete remote file/directory (recursively if `--r` is set).

## Flags
galaxy allows users to set following flags to customize server (mainly) and the client. These flags are defined in [galaxy_flag,h](https://github.com/kfrancischen/galaxy/blob/master/cpp/core/galaxy_flag.h), and their definitions are at [galaxy_flag.cc](https://github.com/kfrancischen/galaxy/blob/master/cpp/core/galaxy_flag.cc). For servers the flags of `fs_root`, `fs_address`, `fs_password` must be specified, and the values of these flags are usually put in the global configuration file. Besides using the configuration file or using the cmd line fashion [abseil](https://abseil.io/docs/cpp/quickstart) supports, one can also specify the flags by using `GALAXY_${FLAG_NAME}` environment variable. For instance, setting `GALAXY_fs_root=/home` is equivalent to parsing `fs_root=/home` as cmd line argument.

## Extensions

#### Galaxy Viewer

A file browser extension is also implemented under [ext/viewer](https://github.com/kfrancischen/galaxy/tree/master/ext/viewer), which uses the Galaxy Python API and flask. The viewer can be launched with the following cmd

```shellscript
python galaxy_viewer.py --username=test --password=test --port=8000
```
and the viewer is hosted at `0.0.0.0:8000` with the preset username and password for login.

#### Galaxy TTL Cleaner

The galaxy file system is also built in with an [ext/ttl_cleaner](https://github.com/kfrancischen/galaxy/tree/master/ext/ttl_cleaner) extension, where one can specify a path with `ttl=${N}d` or `ttl=${N}h` or `ttl=${N}m` for `N` days, hours, minutes, respectively. Capital letters of `D`, `H` and `M` can also be used. Galaxy will only keep the files within the ttl lifetime in the path if the path is associated with a valid ttl.

To launch the batch, periodic ttl cleaner, please use the following command
```shellscript
bazel run -c opt //ext/ttl_cleaner:galaxy_ttl_cleaner -- --run_every=1
```
The argument of `run_every` means the sleep time (in minute) between adjacent ttl cleaner runs. The deafult value is 10 (minutes).


## Examples
The examples are at folder [example](https://github.com/kfrancischen/galaxy/tree/master/example), and the following is a Python example
```python
from galaxy_py import gclient, gclient_ext
import time


def main():
    gclient.create_dir_if_not_exist("/galaxy/aa-d/Downloads/test_from_python")
    print(gclient.list_dirs_in_dir("/galaxy/aa-d/Downloads"))
    print(gclient.list_dirs_in_dir("/home/pslx/Downloads"))
    print(gclient.get_attr("/home/pslx/Downloads"))
    print(gclient.get_attr("/galaxy/aa-d/Downloads/test_from_python"))
    gclient.create_file_if_not_exist("/galaxy/aa-d/Downloads/test_from_python/test.txt")
    gclient_ext.cp_file("/galaxy/aa-d/Downloads/test_from_python/test.txt",
                        "/galaxy/aa-d/Downloads/test_from_python/test1.txt")

    gclient_ext.mv_file("/galaxy/aa-d/Downloads/test_from_python/test.txt",
                        "/galaxy/aa-d/Downloads/test_from_python/test3.txt")
    print(gclient.list_dirs_in_dir_recursive("/galaxy/aa-d/Downloads/historical_stock_data"))
    print(gclient.list_files_in_dir_recursive("/galaxy/aa-d/Downloads/historical_stock_data"))

    t = time.time()
    gclient.read("/galaxy/aa-d/Downloads/large_test.txt")
    print(time.time() - t)

    data = gclient.read_multiple(["/galaxy/aa-d/Downloads/test_3.txt", "/galaxy/aa-d/Downloads/test_1.txt"])
    for key, val in data.items():
        print(key, val)


if __name__ == "__main__":
    main()

```
The following is a C++ example:
```cpp
/* Example cmd
* GALAXY_fs_global_config=/home/pslx/galaxy/example/cpp/server_config_example.json \
* bazel run -c opt //example/cpp:client_example -- --proto_test=/galaxy/aa-d/Downloads/test1/test.pb
*/

#include <iostream>
#include <string>
#include <vector>

#include "cpp/client.h"
#include "cpp/core/galaxy_flag.h"
#include "glog/logging.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "schema/fileserver.pb.h"

ABSL_FLAG(std::string, mkdir_test, "", "The directory for mkdir test.");
ABSL_FLAG(std::string, rmdir_test, "", "The directory for rmdir test.");
ABSL_FLAG(std::string, createfile_test, "", "The directory for createfile test.");
ABSL_FLAG(std::string, proto_test, "", "The directory for createfile test.");

int main(int argc, char* argv[]) {
    absl::ParseCommandLine(argc, argv);
    FLAGS_log_dir = absl::GetFlag(FLAGS_fs_log_dir);
    google::InitGoogleLogging(argv[0]);
    if (!absl::GetFlag(FLAGS_mkdir_test).empty()) {
        galaxy::client::CreateDirIfNotExist(absl::GetFlag(FLAGS_mkdir_test));
    }
    if (!absl::GetFlag(FLAGS_rmdir_test).empty()) {
        galaxy::client::RmDir(absl::GetFlag(FLAGS_rmdir_test));
    }
    if (!absl::GetFlag(FLAGS_createfile_test).empty()) {
        galaxy::client::CreateFileIfNotExist(absl::GetFlag(FLAGS_createfile_test));
        galaxy::client::Write(absl::GetFlag(FLAGS_createfile_test), "hello world");
        std::cout << galaxy::client::Read(absl::GetFlag(FLAGS_createfile_test)) << std::endl;
        std::cout << galaxy::client::Read("/galaxy/aa-d/some_random_file") << std::endl;
    }
    if (!absl::GetFlag(FLAGS_proto_test).empty()) {
        galaxy_schema::Credential cred;
        cred.set_password("test");
        std::string cred_str;
        cred.SerializeToString(&cred_str);
        galaxy::client::Write(absl::GetFlag(FLAGS_proto_test), cred_str);
        std::string result = galaxy::client::Read(absl::GetFlag(FLAGS_proto_test));
        galaxy_schema::Credential result_cred;
        result_cred.ParseFromString(result);
        std::cout << result_cred.DebugString() << std::endl;
    }

    return 0;
}
```
`Python` APIs are more recommended.
