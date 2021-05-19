from galaxy_py import gclient, gclient_ext
import time


def main():
    gclient.create_dir_if_not_exist("/galaxy/aa-d/test_from_python")
    print(gclient.list_dirs_in_dir("/galaxy/aa-d/"))
    print(gclient.list_dirs_in_dir("/home/pslx/Downloads"))
    print(gclient.get_attr("/home/pslx/Downloads"))
    print(gclient.get_attr("/galaxy/aa-d/test_from_python"))
    gclient.create_file_if_not_exist("/galaxy/aa-d/test_from_python/test.txt")
    gclient_ext.cp_file("/galaxy/aa-d/test_from_python/test.txt",
                        "/galaxy/aa-d/test_from_python/test1.txt")

    gclient_ext.mv_file("/galaxy/aa-d/test_from_python/test.txt",
                        "/galaxy/aa-d/test_from_python/test3.txt")
    print(gclient.list_dirs_in_dir_recursive("/galaxy/aa-d/historical_stock_data"))
    print(gclient.list_files_in_dir_recursive("/galaxy/aa-d/historical_stock_data"))

    t = time.time()
    gclient.read("/galaxy/aa-d/large_test.txt")
    print(time.time() - t)

    data = gclient.read_multiple(["/galaxy/aa-d/test_3.txt", "/galaxy/aa-d/test_1.txt"])
    for key, val in data.items():
        print(key, val)


if __name__ == "__main__":
    main()
