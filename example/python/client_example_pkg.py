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


if __name__ == "__main__":
    main()
