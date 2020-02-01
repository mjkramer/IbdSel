import os, sys, time, io
from functools import wraps
from subprocess import check_output

DEFAULT_JOB_CHUNKSIZE = 500

def parse_path(path):
    fields = os.path.basename(path).split('.')
    runno, fileno, site = int(fields[2]), int(fields[6][1:]), int(fields[4][2])
    return runno, fileno, site

def sysload():
    print('BEGIN-SYSLOAD')
    os.system('date; top ibn1; free -g')
    print('END-SYSLOAD')

def unbuf_stdout():
    sys.stdout = io.TextIOWrapper(open(sys.stdout.fileno(), 'wb', 0),
                                  write_through=True)

def phase_for_run(runno):
    if 21221 <= runno <= 26694:
        return 1
    if 34523 <= runno <= 67012:
        return 2
    if runno >= 67625:
        return 3
    raise "Nonsensical run number"

def phase_for_day(day):
    if 0 <= day <= 217:
        return 1
    if 300 <= day <= 1824:
        return 2
    if 1860 <= day <= 2076:
        return 3
    raise ValueError(f'Day {day} is not part of P17B')

def dets_for(site, runno):
    phase = phase_for_run(runno)
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

def _call_bash(cmd):
    fullcmd = f'source bash/common_vars.inc.sh && {cmd}'
    return check_output(fullcmd, shell=True).strip().decode()

def input_fname(step, tag):
    return _call_bash(f'input_file_for {step} {tag}')

def data_dir(name, tag):
    return _call_bash(f'data_dir_for {name} {tag}')

def job_chunksize():
    return int(os.getenv('IBDSEL_CHUNKSIZE', str(DEFAULT_JOB_CHUNKSIZE)))

def _walltime_mins():
    v = os.getenv('IBDSEL_WALLTIME', '00:00:00')
    fields = [int(x) for x in v.split(':')]
    return 60 * fields[0] + fields[1] + fields[2] / 60

def _startup_sleep_mins():
    return int(os.getenv('IBDSEL_TRUE_SLEEP_SECS', '0')) / 60

def _timeout_mins(margin_var_secs):
    margin_mins = int(os.getenv(margin_var_secs, '0')) / 60

    return _walltime_mins() - _startup_sleep_mins() - margin_mins

def buffer_timeout_mins():
    return _timeout_mins('IBDSEL_CHUNK_MARGIN_SECS')

def worker_timeout_mins():
    return _timeout_mins('IBDSEL_FILE_MARGIN_SECS')

def stage1_fbf_path(site, runno, fileno, tag):
    subdir = runno // 100 * 100
    return os.path.join(data_dir('stage1_fbf', tag),
                        f'EH{site}', f'{subdir:07d}', f'{runno:07d}',
                        f'stage1.fbf.eh{site}.{runno:07d}.{fileno:04d}.root')

def stage1_dbd_path(site, day, tag):
    return os.path.join(data_dir('stage1_dbd', tag),
                        f'EH{site}',
                        f'stage1.dbd.eh{site}.{day:04d}.root')
