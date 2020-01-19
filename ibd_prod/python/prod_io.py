import time, os
from itertools import islice

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
    def __init__(self, filename, chunksize=1, timeout_secs=None, retry_delay=5):
        super().__init__()

        self._filename = filename
        self._chunksize = chunksize
        self._cache = []
        self._retry_delay = retry_delay

        self._timeout = timeout_secs
        if self._timeout:
            self._tstart = time.time()

    def __next__(self):
        if not self._cache:
            self._load()
            if not self._cache:
                raise StopIteration
        return self._cache.pop(0).strip()

    def _load(self):
        if self._timeout:
            delta = time.time() - self._tstart
            if delta > self._timeout:
                print('Terminating due to specified timeout')
                raise StopIteration

        print('Grabbing input list lock')
        os.system('time lockfile -%d %s.lock' % (self._retry_delay, self._filename))
        with open(self._filename) as f:
            self._cache = list(islice(f, self._chunksize))
            rest = list(f)
        open(self._filename, 'w').writelines(rest)
        os.system('rm -f %s.lock' % self._filename)

class LockfileListWriter(ListWriterBase):
    def __init__(self, filename, chunksize=1, retry_delay=5):
        super().__init__()

        self._filename = filename
        self._chunksize = chunksize
        self._cache = []
        self._retry_delay = retry_delay

    def _do_put(self, line):
        self._cache.append(line)
        if len(self._cache) == self._chunksize:
            self._flush()

    def close(self):
        if self._cache:
            self._flush()

    def _flush(self):
        chunk = '\n'.join(self._cache) + '\n'
        os.system('time lockfile -%d %s.lock' % (self._retry_delay, self._filename))
        with open(self._filename, 'a') as f:
            f.write(chunk)
        os.system('rm -f %s.lock' % self._filename)
        self._cache = []
