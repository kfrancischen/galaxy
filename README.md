# galaxy
Simple distributed file system based on gRPC.

This is a toy-version of distributed file system developed based on gRPC, and it will be integrated with [pslx](https://github.com/kfrancischen/pslx) shortly. The core logic is developed with C++ and later wrapped with Python using [Pybind11](https://github.com/pybind/pybind11). The build system for the whole package is [bazel](https://bazel.build/). In addition, several public versions of Google internal tools are used: [abseil](https://abseil.io/docs/cpp/quickstart), [glog](https://github.com/google/glog). [rapidjson](https://rapidjson.org/) is used to parse the config files.

#### Core concepts
- `cell`: a cell is a machine that can be added as part of the filesystem, and is associated with a cell name. In galaxy, we use a two letter name for a cell, i.e. `aa`. The path for a cell in galaxy filesystem starts with `/galaxy/${CELL}-d/...`, where the `${CELL}` is the name of the cell. To make a machine as a cell in the filesystem, one just needs to launch the server code on the machine. Details are discussed in the next section.
- `global config`: a configuration file containing configurations for each cell in the galaxy filesystem. An example of it is in [server_config_example.json](https://github.com/kfrancischen/galaxy/blob/master/example/cpp/server_config_example.json).

#### Server Entry Points
The entry point for galaxy filesystem server is located at [server_main.cc](https://github.com/kfrancischen/galaxy/blob/master/cpp/server_main.cc). To run the binary, one can use the following cmd
```shell script
bazel run //cpp:galaxy_server -- \
--fs_global_config=/home/pslx/galaxy/example/cpp/server_config_example.json \
--fs_cell=aa
```
With the above cmd, the machine is added as cell `aa` with configurations specified in the `json` file.

#### Client Python API
galaxy provides unified API for client to access both local and remote files, to build the python modules, please following the cmd of
```shell script
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
* Decription: remove an empty directory.
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
* Decription: read a file.
* Args:
    1. path: the path to the file

```python
write(path, data, mode="w")
```
* Decription: write data to a file.
* Args:
    1. path: the path to the file
    2. data: the data in string format
    3. mode: `w` means overwrite and `a` means append.

```python
read_large(path)
```
* Decription: read a large file.
* Args:
    1. path: the path to the file

```python
write_large(path, data, mode="w")
```
* Decription: write large data to a file.
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
read_proto_message(path)
```
* Decription: read a protobuf message file.
* Args:
    1. path: the path to the protobuf message file

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

#### Fileutil tool
TO BE ADDED

#### Flags
galaxy allows users to set following flags to customize server (mainly) and the client. These flags are defined in [galaxy_flag,h](https://github.com/kfrancischen/galaxy/blob/master/cpp/core/galaxy_flag.h), and their definitions are at [galaxy_flag.cc](https://github.com/kfrancischen/galaxy/blob/master/cpp/core/galaxy_flag.cc). For servers the flags of `fs_root`, `fs_address`, `fs_password` must be specified, and the values of these flags are usually put in the global configuration file. Besides using the configuration file or using the cmd line fashion [abseil](https://abseil.io/docs/cpp/quickstart) supports, one can also specify the flags by using `GALAXY_${FLAG_NAME}` environment variable. For instance, setting `GALAXY_fs_root=/home` is equivalent to parsing `fs_root=/home` as cmd line argument.

#### Examples
The examples are at folder [example](https://github.com/kfrancischen/galaxy/tree/master/example), and the following is a Python example
```python

from galaxy_py import gclient, gclient_ext


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
    gclient_ext.cp_folder("/galaxy/aa-d/Downloads/historical_stock_data",
                          "/galaxy/aa-d/Downloads/historical_stock_data_copy")
    gclient_ext.mv_folder("/galaxy/aa-d/Downloads/historical_stock_data_copy",
                          "/galaxy/aa-d/Downloads/historical_stock_data_copy1")

if __name__ == "__main__":
    main()
```
