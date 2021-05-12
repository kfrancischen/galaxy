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
    # gclient_ext.cp_folder_large("/galaxy/aa-d/Downloads/historical_stock_data",
    #                             "/galaxy/aa-d/Downloads/historical_stock_data_copy")
    t = time.time()
    gclient.read_large("/galaxy/aa-d/Downloads/large_test.txt")
    print(time.time() - t)
    t = time.time()
    gclient.read("/galaxy/aa-d/Downloads/large_test.txt")
    print(time.time() - t)
    # gclient_ext.mv_folder("/galaxy/aa-d/Downloads/historical_stock_data_copy",
    #                       "/galaxy/aa-d/Downloads/historical_stock_data_copy1")


if __name__ == "__main__":
    main()
