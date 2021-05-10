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
        if data:
            proto_message = message_type()
            proto_message.ParseFromString(data)
            return proto_message
        else:
            return None

    @classmethod
    def list_all_in_dir(cls, path):
        return gclient.list_dirs_in_dir(path) + gclient.list_files_in_dir(path)

    @classmethod
    def cp_file(cls, from_path, to_path):
        if not gclient.file_or_die(from_path):
            return

        gclient.create_file_if_not_exist(to_path)
        data = gclient.read(from_path)
        if data:
            gclient.write(to_path, data)

    @classmethod
    def mv_file(cls, from_path, to_path):
        if not gclient.file_or_die(from_path):
            return

        gclient.create_file_if_not_exist(to_path)
        data = gclient.read(from_path)
        if data:
            gclient.write(to_path, data)

        gclient.rm_file(from_path)


__all__ = [
    "gclient",
    "gclient_ext",
]
