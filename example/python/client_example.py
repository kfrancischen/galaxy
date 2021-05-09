"""
Example cmd:
GALAXY_fs_global_config=/home/pslx/galaxy/example/cpp/server_config_example.json bazel run //example/python:client_example
"""

from python import gclient as gclient


def main():
    gclient.create_dir_if_not_exist("/galaxy/aa-d/Download/test_from_python")
    print(gclient.list_dirs_in_dir("/galaxy/aa-d/Download"))
    print(gclient.dir_or_die("/galaxy/aa-d/Download"))
    print(gclient.llist_dirs_in_dir("/home/pslx/Download"))


if __name__ == "__main__":
    main()
