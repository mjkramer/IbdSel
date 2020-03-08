#!/usr/bin/env python3

# takes 7 minutes to do p17b

import argparse, os
from glob import glob
from collections import defaultdict

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

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('tag')
    ap.add_argument('config')
    args = ap.parse_args()

    merge(args.tag, args.config)

if __name__ == '__main__':
    unbuf_stdout()
    main()
