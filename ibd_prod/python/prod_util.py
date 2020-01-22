import os, sys, time
from functools import wraps

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

def phase_for(runno):
    if 21221 <= runno <= 26694:
        return 1
    if 34523 <= runno <= 67012:
        return 2
    if runno >= 67625:
        return 3
    raise "Nonsensical run number"

def dets_for(site, runno):
    phase = phase_for(runno)
    if site == 1:
        return [2] if phase == 3 else [1, 2]
    if site == 2:
        return [1] if phase == 1 else [1, 2]
    if site == 3:
        return [1, 2, 3] if phase == 1 else [1, 2, 3, 4]
    raise ValueError("Invalid site")

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
