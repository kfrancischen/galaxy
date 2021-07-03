from galaxy_py import gclient, gclient_ext
from hello_world_pb2 import TestMessage
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
    print(gclient.list_dirs_in_dir_recursive("/galaxy/aa-d/test_from_python"))
    print(gclient.list_files_in_dir_recursive("/galaxy/aa-d/test_from_python"))

    t = time.time()
    gclient.read("/galaxy/aa-d/large_test.txt")
    print(time.time() - t)

    data = gclient.read_multiple(["/galaxy/aa-d/test_from_python/test1.txt", "/galaxy/aa-d/test3.txt"])
    for key, val in data.items():
        print(key, val)

    data = gclient_ext.read_txts(["/galaxy/aa-d/test_from_python/test1.txt", "/galaxy/aa-d/test3.txt"])
    for key, val in data.items():
        print(key, val)

    print(gclient.list_cells())
    print(gclient.read('/galaxy/aa-d/test.pb'))
    print(gclient_ext.read_txt('/galaxy/aa-d/test.pb'))
    message = TestMessage()
    message.name = "test"
    gclient_ext.write_proto_message('/galaxy/aa-d/test1.pb', message)
    print(gclient_ext.read_proto_message('/galaxy/aa-d/test1.pb', TestMessage))
    print(gclient.check_health("aa"))

    gclient.write_multiple(
        path_data_map={
            '/galaxy/aa-d/test_from_python/test2.txt': '123',
            '/galaxy/aa-d/test_from_python/test4.txt': '234',
        },
        mode='a'
    )
    gclient_ext.write_proto_messages(
        path_data_map={
            '/galaxy/aa-d/test_from_python/test2.pb': message,
            '/galaxy/aa-d/test_from_python/test4.pb': message,
        }
    )
    print(gclient_ext.read_proto_messages(
        paths=[
          '/galaxy/aa-d/test_from_python/test2.pb', '/galaxy/aa-d/test_from_python/test4.pb'],
        message_type=TestMessage))


if __name__ == "__main__":
    main()
