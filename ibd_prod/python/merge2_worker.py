#!/usr/bin/env python3

# takes 7 minutes to do p17b
# TODO merge the three halls in parallel

import argparse
from collections import defaultdict
from glob import glob
import os
import sys

from prod_io import LockfileListReader, LockfileListWriter
from prod_util import data_dir, phase_for_day, stage2_pbp_path
from prod_util import unbuf_stdout
from hadd import hadd_chunked

HADD_CHUNKSIZE = 100

def gather(site, tag, config):
    results = defaultdict(lambda: [])
    pat = os.path.join(data_dir('stage2_dbd', f'{tag}@{config}'),
                       f'EH{site}', '*.root')
    for f in glob(pat):
        day = int(os.path.basename(f).split('.')[3])
        phase = phase_for_day(day)
        results[phase].append(f)

    return results

def merge(tag, config):
    for site in [1, 2, 3]:
        filesets = gather(site, tag, config)

        for phase in [1, 2, 3]:
            files = sorted(filesets[phase])
            outfile = stage2_pbp_path(site, phase, tag, config)
            os.makedirs(os.path.dirname(outfile), exist_ok=True)

            hadd_chunked(files, outfile, chunksize=HADD_CHUNKSIZE)

def copy_config(tag, config):
    src = os.path.join(data_dir('stage2_dbd', f'{tag}@{config}'),
                       f'config.{config}.txt')
    dest = data_dir('stage2_pbp', f'{tag}@{config}')
    os.system(f'cp {src} {dest}')

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('-l', '--listfile',
                    help='File with each line like: 2020_01_26 nominal')
    ap.add_argument('tag', nargs='?')
    ap.add_argument('config', nargs='?')
    args = ap.parse_args()

    def do_merge(tag, config):
        merge(tag, config)
        copy_config(tag, config)

    if args.listfile:
        reader = LockfileListReader(args.listfile, chunksize=1)
        writer = LockfileListWriter(args.listfile + ".done", chunksize=1)

        with writer:
            for line in reader:
                tag, config = line.strip().split()

                do_merge(tag, config)

                writer.log(line)

    if not args.listfile:
        assert args.tag and args.config
        do_merge(args.tag, args.config)


if __name__ == '__main__':
    unbuf_stdout()
    main()
