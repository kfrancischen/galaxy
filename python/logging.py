import time
import logging
import threading
import struct
import traceback
import os
from inspect import getframeinfo, stack

try:
    from python import _gclient as gclient
except ImportError:
    import _gclient as gclient

_THREAD_ID_MASK = 2 ** (struct.calcsize('L') * 8) - 1
_CRITICAL_PREFIX = 'CRITICAL - '

_CALL_STACK = {
    logging.INFO: 11,
    logging.WARNING: 12,
    logging.ERROR: 11,
    logging.CRITICAL: 11,
    logging.FATAL: 11,
}


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

        try:
            caller = getframeinfo(stack()[_CALL_STACK[level]][0])
            filename = caller.filename.split('/')[-1]
            lineno = caller.lineno
        except Exception as _:
            filename = record.filename
            lineno = record.lineno

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
            filename,
            lineno,
            critical_prefix)

    def format(self, record):
        return_msg = GalaxyLoggingFormatter.get_log_prefix(record) + super(GalaxyLoggingFormatter, self).format(record)
        if record.levelno >= logging.ERROR:
            return_msg += '\n\nENTERING TRACEBACK:\n' + traceback.format_exc()
        return return_msg


class GalaxyLoggingHandler(logging.StreamHandler):

    def __init__(self, filename_prefix):
        super().__init__()
        assert filename_prefix and filename_prefix[-1] != '/', "Wrong format of the log file prefix."
        self._prefix = filename_prefix
        self.setFormatter(GalaxyLoggingFormatter())

    def get_file_name(self, record):
        levelno = record.levelno
        cur_date = time.strftime('%Y-%m-%d', time.localtime(time.time()))
        if levelno == logging.INFO or levelno == logging.DEBUG:
            return '.'.join([self._prefix, cur_date, 'INFO', 'log'])
        if levelno == logging.WARNING:
            return '.'.join([self._prefix, cur_date, 'WARNING', 'log'])
        if levelno == logging.ERROR:
            return '.'.join([self._prefix, cur_date, 'ERROR', 'log'])
        else:
            return '.'.join([self._prefix, cur_date, 'FATAL', 'log'])

    def emit(self, record):
        msg = self.format(record)
        file_name = self.get_file_name(record)
        gclient.write(file_name, msg + '\n', 'a')
        super(GalaxyLoggingHandler, self).emit(record)


class GalaxyLogger(object):

    def __init__(self, log_name, logfile_dir, level=logging.INFO):
        super().__init__()
        self._logger = logging.getLogger(log_name)
        self._logger.setLevel(level)
        handler = GalaxyLoggingHandler(os.path.join(logfile_dir, log_name))
        handler.setLevel(level)
        self._logger.addHandler(handler)

    def get_logger(self):
        return self._logger

    def info(self, msg):
        self._logger.info(msg)

    def warn(self, msg):
        self._logger.warn(msg)

    def error(self, msg):
        self._logger.error(msg)

    def fatal(self, msg):
        self._logger.fatal(msg)

    def critical(self, msg):
        self._logger.critical(msg)

    def debug(self, msg):
        self._logger.debug(msg)
