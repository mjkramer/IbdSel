#!/usr/bin/env python3

import argparse
import os
from glob import glob
from dataclasses import dataclass

LOGBASE = '../../log'


def logbase(stage):
    return os.path.join(LOGBASE, stage)


@dataclass(frozen=True)
class BenchmarkId:
    sysname: str
    numfiles: int
    uuid: str
    prefix: str = ''

    def base(self):
        prefix = f'{self.prefix}@' if self.prefix else ''
        return f'{prefix}bm_{self.sysname}.{self.numfiles}.{self.uuid}'

    def __str__(self):
        return f'{self.sysname}/{self.numfiles}/{self.uuid}'


def get_runtime(logfile):
    def read_stamp(line):
        return int(line.split()[2])
    beg, end = None, None
    for line in open(logfile):
        if line.startswith('Beginning at '):
            beg = read_stamp(line)
        elif line.startswith('Ending at '):
            end = read_stamp(line)
    if None in [beg, end]:
        return None
    return end - beg


def dump_times(stage, bmId: BenchmarkId, prefix=None):
    pat = os.path.join(logbase(stage), bmId.base()) + '.*'
    dirs = sorted(glob(pat))

    print(f'# Results for {bmId}:')

    for d in dirs:
        logfiles = glob(os.path.join(d, 'slurm-*.out'))
        if len(logfiles) != 1:
            continue

        ntasks = int(d.split('.')[-1])
        runtime = get_runtime(logfiles[0])
        if runtime is None:
            score = "SLOW"
        else:
            secperfile = runtime / bmId.numfiles
            score = f'{secperfile:.02f} node-seconds per file'

        print(f'{ntasks:2} tasks: {score}')


def all_benchmarks(stage, sysname=None):
    seen = set()
    pat = os.path.join(logbase(stage), '*bm_*')
    for d in sorted(glob(pat)):
        parts = os.path.basename(d).split('.')
        if parts[0].find('@') != -1:
            prefix, name = parts[0].split('@')
        else:
            prefix, name = '', parts[0]
        sysname = name.split('_')[1]
        numfiles = int(parts[1])
        uuid = parts[2]
        bmId = BenchmarkId(sysname, numfiles, uuid, prefix)
        if bmId not in seen:
            seen.add(bmId)
            yield bmId


def dump_all(stage, filters):
    i = 0
    for bmId in all_benchmarks(stage, filters):
        if any(not f(bmId) for f in filters):
            continue

        if i != 0:
            print()
        i += 1

        dump_times(stage, bmId)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('stage')
    ap.add_argument('-u', '--uuid')
    ap.add_argument('-s', '--sysname')
    ap.add_argument('-n', '--numfiles')
    ap.add_argument('-p', '--prefix', help='e.g. the tag')
    args = ap.parse_args()

    filters = []

    if args.uuid:
        filters.append(lambda bmId: bmId.uuid == args.uuid)
    if args.sysname:
        filters.append(lambda bmId: bmId.sysname == args.sysname)
    if args.numfiles:
        filters.append(lambda bmId: bmId.numfiles == args.numfiles)
    if args.prefix:
        filters.append(lambda bmId: bmId.prefix == args.prefix)

    dump_all(args.stage, filters)


if __name__ == '__main__':
    main()
