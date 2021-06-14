try:
    from python import _gclient as gclient
except ImportError:
    import _gclient as gclient

try:
    from python.logging import glogging
except ImportError:
    from galaxy_py.logging import glogging


class gclient_ext:

    @classmethod
    def write_proto_message(cls, path, data):
        gclient.write(path=path, data=data.SerializeToString())

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
    def read_txt(cls, path):
        data = gclient.read(path)
        if data:
            return data.decode('UTF-8')
        else:
            return None

    @classmethod
    def read_proto_messages(cls, paths, message_type):
        data_map = gclient.read_multiple(paths)
        result_map = {}
        for key, val in data_map.items():
            if val:
                proto_message = message_type()
                proto_message.ParseFromString(val)
                result_map[key] = proto_message
            else:
                result_map[key] = None
        return result_map

    @classmethod
    def read_txts(cls, paths):
        data_map = gclient.read_multiple(paths)
        result_map = {}
        for key, val in data_map.items():
            if val:
                result_map[key] = val.decode('UTF-8')
            else:
                result_map[key] = None
        return result_map

    @classmethod
    def list_all_in_dir(cls, path):
        return {**gclient.list_dirs_in_dir(path), **gclient.list_files_in_dir(path)}

    @classmethod
    def list_all_in_dir_recursive(cls, path):
        return {**gclient.list_dirs_in_dir_recursive(path), **gclient.list_files_in_dir_recursive(path)}

    @classmethod
    def cp_file(cls, from_path, to_path):
        import warnings
        warnings.warn("Please consider using fileutil binary for better performance.", DeprecationWarning)
        if not gclient.file_or_die(from_path):
            return

        gclient.create_file_if_not_exist(to_path)
        data = gclient.read(from_path)
        if data:
            gclient.write(to_path, data)

    @classmethod
    def mv_file(cls, from_path, to_path):
        import warnings
        warnings.warn("Please consider using fileutil binary for better performance.", DeprecationWarning)
        if not gclient.file_or_die(from_path):
            return

        gclient.create_file_if_not_exist(to_path)
        data = gclient.read(from_path)
        if data:
            gclient.write(to_path, data)

        gclient.rm_file(from_path)

    @classmethod
    def cp_folder(cls, from_path, to_path):
        import warnings
        warnings.warn("Please consider using fileutil binary for better performance.", DeprecationWarning)
        if not gclient.dir_or_die(from_path):
            return

        all_files = gclient.list_files_in_dir_recursive(from_path)
        for old_file in all_files:
            new_file = old_file.replace(from_path, to_path)
            cls.cp_file(old_file, new_file)

    @classmethod
    def mv_folder(cls, from_path, to_path):
        import warnings
        warnings.warn("Please consider using fileutil binary for better performance.", DeprecationWarning)
        if not gclient.dir_or_die(from_path):
            return

        all_files = gclient.list_files_in_dir_recursive(from_path)
        for old_file in all_files:
            new_file = old_file.replace(from_path, to_path)
            cls.mv_file(old_file, new_file)
        gclient.rm_dir_recursive(from_path)


__all__ = [
    "gclient",
    "gclient_ext",
    "glogging",
]
