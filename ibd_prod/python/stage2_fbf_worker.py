#!/usr/bin/env python3

# This is intended for diagnostsics and debugging
# Normally we run stage2 on day-by-day files

import os, argparse, random

from prod_util import unbuf_stdout
from prod_io import LockfileListReader, LockfileListWriter

def process(fbf1_path, outdir, phase, configpath):
    parts = fbf1_path.split(".")
    site = int(parts[-4][2])
    runno = int(parts[-3])
    fileno = int(parts[-2])

    outpath = f"{outdir}/stage2.fbf.eh{site}.{runno:07d}.{fileno:04d}"
    seq = runno * 10000 + fileno

    exe = os.getenv('IBDSEL_HOME') + '/selector/_build/stage2.exe'
    cmd = f'{exe} {configpath} {fbf1_path} {outpath} {site} {phase} {seq}'
    print(cmd)
    os.system(f'time {cmd}')


def nominal_configpath():
    home = os.environ["IBDSEL_HOME"]
    path = f"{home}/static/configs/config.nominal.txt"
    assert os.path.exists(path)
    return path


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('listfile')
    ap.add_argument('outdir')
    ap.add_argument('phase', help='1=6AD, 2=8AD, 3=7AD')
    ap.add_argument('--configpath', default=nominal_configpath())
    args = ap.parse_args()

    reader = LockfileListReader(args.listfile, chunksize=1)
    logger = LockfileListWriter(args.listfile + ".done", chunksize=1)

    with logger:
        for line in reader:
            fbf1_path = line.strip()
            process(fbf1_path, args.outdir, args.phase, args.configpath)
            logger.log(line)

if __name__ == '__main__':
    unbuf_stdout()
    main()
