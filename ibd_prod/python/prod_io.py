import time, os

class ListReaderBase:
    def __init__(self, timeout_mins=None):
        self._timeout_mins = timeout_mins

        if self._timeout_mins:
            self._tstart = time.time()

    def __iter__(self):
        return self

    def __next__(self):
        raise NotImplementedError

    def _check_timeout(self):
        if self._timeout_mins:
            delta = (time.time() - self._tstart) / 60
            if delta > self._timeout_mins:
                print('Terminating due to specified timeout')
                raise StopIteration

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
    def __init__(self, filename, chunksize=1, retry_delay=5, **kwargs):
        super().__init__(**kwargs)

        self._filename = filename
        self._chunksize = chunksize
        self._retry_delay = retry_delay

        self._all = []
        self._working = []
        self._lastmtime = 0

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
        open(self._offset_file(), 'w').write(f'{offset}\n')

    def _pull(self):
        self._check_timeout()

        print('Grabbing input list lock')
        os.system('time lockfile -%d %s' % (self._retry_delay, self._lock_file()))

        if self._modified():
            self._load()

        offset = self._read_offset()

        if offset < len(self._all):
            self._write_offset(offset + self._chunksize)

        os.system('rm -f %s' % self._lock_file())

        self._working = self._all[offset : offset + self._chunksize]

class LockfileListWriter(ListWriterBase):
    def __init__(self, filename, chunksize=1, retry_delay=5, **kwargs):
        super().__init__(**kwargs)

        self._filename = filename
        self._chunksize = chunksize
        self._retry_delay = retry_delay

        self._buf = []

    def _do_put(self, line):
        self._buf.append(line)
        if len(self._buf) == self._chunksize:
            self._flush()

    def close(self):
        if self._buf:
            self._flush()

    def _flush(self):
        chunk = '\n'.join(self._buf) + '\n'
        os.system('time lockfile -%d %s.lock' % (self._retry_delay, self._filename))
        with open(self._filename, 'a') as f:
            f.write(chunk)
        os.system('rm -f %s.lock' % self._filename)
        self._buf = []
