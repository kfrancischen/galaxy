import time
import logging
import threading
import struct
import traceback
import os

try:
    from python import _gclient as gclient
except ImportError:
    import _gclient as gclient

_THREAD_ID_MASK = 2 ** (struct.calcsize('L') * 8) - 1
_CRITICAL_PREFIX = 'CRITICAL - '
_TRACEBACK_SEP = '-------------------'


class GalaxyLoggingFormatter(logging.Formatter):

    @staticmethod
    def get_log_prefix(record):
        critical_prefix = ''
        level = record.levelno
        if level < logging.WARNING:
            severity = 'I'
        elif level < logging.ERROR:
            severity = 'W'
        elif level < logging.CRITICAL:
            severity = 'E'
        else:
            critical_prefix = _CRITICAL_PREFIX
            severity = 'F'

        created_tuple = time.localtime(record.created)
        created_microsecond = int(record.created % 1.0 * 1e6)
        thread_id = threading.get_ident() & _THREAD_ID_MASK
        return '%c%04d%02d%02d %02d:%02d:%02d.%06d %5d %s:%d] %s' % (
            severity,
            created_tuple.tm_year,
            created_tuple.tm_mon,
            created_tuple.tm_mday,
            created_tuple.tm_hour,
            created_tuple.tm_min,
            created_tuple.tm_sec,
            created_microsecond,
            thread_id,
            record.filename,
            record.lineno,
            critical_prefix)

    def format(self, record):
        return_msg = GalaxyLoggingFormatter.get_log_prefix(
            record) + super(GalaxyLoggingFormatter, self).format(record)
        if record.levelno >= logging.ERROR:
            return_msg += '\n' + _TRACEBACK_SEP + '\nENTERING TRACEBACK:\n' + \
                traceback.format_exc() + _TRACEBACK_SEP
        return return_msg


class GalaxyLoggingHandler(logging.StreamHandler):

    def __init__(self, log_prefix):
        super().__init__()
        assert log_prefix and log_prefix[-1] != '/', "Wrong format of the input."
        self._prefix = log_prefix
        self._filename = None
        self.setFormatter(GalaxyLoggingFormatter())

    def _get_file_name_from_record(self, record):
        levelno = record.levelno
        cur_date = time.strftime('%Y-%m-%d', time.localtime(time.time()))
        if levelno == logging.INFO or levelno == logging.DEBUG:
            level_file = '.'.join([self._prefix, cur_date, 'INFO', 'log'])
        elif levelno == logging.WARNING:
            level_file = '.'.join([self._prefix, cur_date, 'WARNING', 'log'])
        elif levelno == logging.ERROR:
            level_file = '.'.join([self._prefix, cur_date, 'ERROR', 'log'])
        else:
            level_file = '.'.join([self._prefix, cur_date, 'FATAL', 'log'])
        return level_file, '.'.join([self._prefix, cur_date, 'ALL', 'log'])

    def get_file_name(self):
        return self._filename

    def emit(self, record):
        msg = self.format(record)
        level_file, self._filename = self._get_file_name_from_record(record)
        gclient.write(level_file, msg + '\n', 'a')
        gclient.write(self._filename, msg + '\n', 'a')
        super(GalaxyLoggingHandler, self).emit(record)


class glogging(object):

    @classmethod
    def get_logger(cls, log_name, log_dir):
        logger = logging.getLogger(log_name)
        logger.setLevel(logging.DEBUG)
        handler = GalaxyLoggingHandler(os.path.join(log_dir, log_name))
        handler.setLevel(logging.DEBUG)
        logger.addHandler(handler)
        return logger

    @classmethod
    def get_logger_file(cls, logger):
        try:
            return logger.handlers[0].get_file_name()
        except Exception as _:
            return ""
