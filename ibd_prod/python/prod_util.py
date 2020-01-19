import os, sys, time
from itertools import islice
from functools import wraps

class ParallelListReader:
    def __init__(self, filename, chunksize=1, timeout_secs=None, retry_delay=5):
        self._filename = filename
        self._chunksize = chunksize
        self._cache = []
        self._retry_delay = retry_delay

        self._timeout = timeout_secs
        if self._timeout:
            self._tstart = time.time()

    def __iter__(self):
        return self

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

class ParallelListWriter:
    def __init__(self, filename, chunksize=1, retry_delay=5):
        self._filename = filename
        self._chunksize = chunksize
        self._cache = []
        self._retry_delay = retry_delay

    def put(self, line):
        self._cache.append(line)
        if len(self._cache) == self._chunksize:
            self._flush()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if self._cache:
            self._flush()

    def _flush(self):
        chunk = '\n'.join(self._cache) + '\n'
        os.system('time lockfile -%d %s.lock' % (self._retry_delay, self._filename))
        with open(self._filename, 'a') as f:
            f.write(chunk)
        os.system('rm -f %s.lock' % self._filename)
        self._cache = []

class DoneLogger(ParallelListWriter):
    def log(self, path):
        tstamp = time.strftime('%Y-%m-%dT%H:%M:%S')
        self.put(tstamp + ' ' + path)

def parse_path(path):
    fields = os.path.basename(path).split('.')
    runno, fileno, site = int(fields[2]), int(fields[6][1:]), int(fields[4][2])
    return runno, fileno, site

def sysload():
    print('BEGIN-SYSLOAD')
    os.system('date; top ibn1; free -g')
    print('END-SYSLOAD')

def unbuf_stdout():
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)

def stage_for(runno):
    if 21221 <= runno <= 26694:
        return 1
    if 34523 <= runno <= 67012:
        return 2
    if 67625 <= runno:
        return 3
    raise "Nonsensical run number"

def dets_for(site, runno):
    stage = stage_for(runno)
    if site == 1:
        return [2] if stage == 3 else [1, 2]
    if site == 2:
        return [1] if stage == 1 else [1, 2]
    if site == 3:
        return [1, 2, 3] if stage == 1 else [1, 2, 3, 4]

def log_time(fn):
    @wraps(fn)
    def wrapper(*args, **kwargs):
        t1 = time.time()
        res = fn(*args, **kwargs)
        t2 = time.time()
        print('ELAPSED %d seconds' % (t2 - t1))
        return res
    return wrapper

def gen2thing(thing):
    def wrap(gen):
        @wraps(gen)
        def fun(*args, **kwargs):
            return thing(gen(*args, **kwargs))
        return fun

    return wrap

gen2list = gen2thing(list)

@gen2list
def chunk_list(L, chunksize):
    for i in range(0, len(L), chunksize):
        yield L[i : i + chunksize]  # yes, kosher even if it points past end
