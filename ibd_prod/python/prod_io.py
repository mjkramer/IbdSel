import time, os
from dataclasses import dataclass
from typing import Optional

class ListReaderBase:
    def __iter__(self):
        return self

    def __next__(self):
        raise NotImplementedError

class ListWriterBase:
    def __init__(self):
        self.__in_context = False

    def __enter__(self):
        self.__in_context = True
        return self

    def __exit__(self, *_exc):
        self.__in_context = False
        self.close()

    def close(self):
        pass

    def _do_put(self, line):
        raise NotImplementedError

    def put(self, line):
        if not self.__in_context:
            raise RuntimeError('ListWriter must be used in "with" block')
        self._do_put(line)

    def log(self, line):
        tstamp = time.strftime('%Y-%m-%dT%H:%M:%S')
        self.put(f'{tstamp} {line}')

class LockfileListReader(ListReaderBase):
    @dataclass
    class Config:
        chunksize: int = 1
        timeout_secs: Optional[int] = None
        retry_delay: int = 5

    def __init__(self, filename, **cfg_kwargs):
        super().__init__()

        self._filename = filename
        self._cfg = LockfileListReader.Config(**cfg_kwargs)

        self._all = []
        self._working = []
        self._lastmtime = 0

        if self._cfg.timeout_secs:
            self._tstart = time.time()

        self._load()

    def _load(self):
        self._all = open(self._filename).readlines()
        self._lastmtime = os.path.getmtime(self._filename)

    def _modified(self):
        return os.path.getmtime(self._filename) > self._lastmtime

    def __next__(self):
        if not self._working:
            self._pull()
            if not self._working:
                raise StopIteration
        return self._working.pop(0).strip()

    def _lock_file(self):
        return self._filename + '.lock'

    def _offset_file(self):
        return self._filename + '.offset'

    def _read_offset(self):
        try:
            return int(open(self._offset_file()).read())
        except FileNotFoundError:
            return 0

    def _write_offset(self, offset):
        open(self._offset_file(), 'w').write(str(offset))

    def _pull(self):
        if self._cfg.timeout_secs:
            delta = time.time() - self._tstart
            if delta > self._cfg.timeout_secs:
                print('Terminating due to specified timeout')
                raise StopIteration

        print('Grabbing input list lock')
        os.system('time lockfile -%d %s' % (self._cfg.retry_delay, self._lock_file()))

        if self._modified():
            self._load()

        offset = self._read_offset()

        if offset < len(self._all):
            self._write_offset(offset + self._cfg.chunksize)

        os.system('rm -f %s' % self._lock_file())

        self._working = self._all[offset : offset + self._cfg.chunksize]

class LockfileListWriter(ListWriterBase):
    @dataclass
    class Config:
        chunksize: int = 1
        retry_delay: int = 5

    def __init__(self, filename, **cfg_kwargs):
        super().__init__()

        self._filename = filename
        self._cfg = LockfileListWriter.Config(**cfg_kwargs)

        self._buf = []

    def _do_put(self, line):
        self._buf.append(line)
        if len(self._buf) == self._cfg.chunksize:
            self._flush()

    def close(self):
        if self._buf:
            self._flush()

    def _flush(self):
        chunk = '\n'.join(self._buf) + '\n'
        os.system('time lockfile -%d %s.lock' % (self._cfg.retry_delay, self._filename))
        with open(self._filename, 'a') as f:
            f.write(chunk)
        os.system('rm -f %s.lock' % self._filename)
        self._buf = []
