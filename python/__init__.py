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


__all__ = [
    "gclient",
    "gclient_ext",
]
