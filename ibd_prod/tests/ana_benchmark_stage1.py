#!/usr/bin/env python3

import os
from glob import glob
from dataclasses import dataclass

LOGBASE = '../../log/stage1'

@dataclass(frozen=True)
class BenchmarkId:
    sysname: str
    numfiles: int
    tag: str

    def base(self):
        return f'benchmark_{self.sysname}.{self.numfiles}.{self.tag}'

    def __str__(self):
        return f'{self.sysname}/{self.numfiles}/{self.tag}'

def get_runtime(logfile):
    read_stamp = lambda line: int(line.split()[2])
    beg, end = None, None
    for line in open(logfile):
        if line.startswith('Beginning at '):
            beg = read_stamp(line)
        elif line.startswith('Ending at '):
            end = read_stamp(line)
    if None in [beg, end]:
        return None
    return end - beg

def dump_times(bmId: BenchmarkId):
    pat = os.path.join(LOGBASE, bmId.base()) + '.*'
    dirs = sorted(glob(pat))

    print(f'Results for {bmId}:')

    for d in dirs:
        logfiles = glob(os.path.join(d, 'slurm-*.out'))
        assert len(logfiles) == 1

        ntasks = int(d.split('.')[-1])
        runtime = get_runtime(logfiles[0])
        if runtime is None:
            score = "SLOW"
        else:
            secperfile = runtime / bmId.numfiles
            score = f'{secperfile:.02f} node-seconds per file'

        print(f'{ntasks} tasks: {score}')

def all_benchmarks():
    seen = set()
    pat = os.path.join(LOGBASE, 'benchmark_*')
    for d in sorted(glob(pat)):
        parts = os.path.basename(d).split('.')
        sysname = parts[0].split('_')[1]
        numfiles = int(parts[1])
        tag = parts[2]
        bmId = BenchmarkId(sysname, numfiles, tag)
        if bmId not in seen:
            seen.add(bmId)
            yield bmId

def dump_all():
    for i, bmId in enumerate(all_benchmarks()):
        if i != 0:
            print()
        dump_times(bmId)
