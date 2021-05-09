try:
    from python import _gclient as gclient
except ImportError:
    import _gclient as gclient


class gclient_ext:

    @classmethod
    def write_proto_message(cls, path, data, mode):
        gclient.write(path=path, data=data.SerializeToString(), mode=mode)

    @classmethod
    def read_proto_message(cls, path, message_type):
        data = gclient.read(path)
        proto_message = message_type()
        proto_message.ParseFromString(data)
        return proto_message

    @classmethod
    def list_all_in_dir(cls, path):
        return gclient.list_dirs_in_dir(path) + gclient.list_files_in_dir(path)

    @classmethod
    def lwrite_proto_message(cls, path, data, mode):
        gclient.lwrite(path=path, data=data.SerializeToString(), mode=mode)

    @classmethod
    def lread_proto_message(cls, path, message_type):
        data = gclient.lread(path)
        proto_message = message_type()
        proto_message.ParseFromString(data)
        return proto_message

    @classmethod
    def llist_all_in_dir(cls, path):
        return gclient.llist_dirs_in_dir(path) + gclient.llist_files_in_dir(path)


__all__ = [
    "gclient",
    "gclient_ext",
]
