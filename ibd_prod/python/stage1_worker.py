#!/usr/bin/env python3

# This code expects the environment variables exported in
# bash/stage1_vars.inc.sh. See bash/stage1_job.sh

import os, argparse, random

from prod_util import parse_path, sysload, phase_for, input_fname, data_dir
from prod_io import LockfileListReader, LockfileListWriter
from zmq_fan import ZmqListReader, ZmqListWriter

def process(path, outdir):
    runno, fileno, site = parse_path(path)
    subdir = runno // 100 * 100
    outpath = os.path.join(outdir, 'EH%d' % site, '%07d' % subdir, '%07d' % runno,
                           'stage1.fbf.eh%d.%07d.%04d.root' % (site, runno, fileno))
    os.system('mkdir -p %s' % os.path.dirname(outpath))
    os.system('time root -l -b -q "cling/run_stage1.C(\\"%s\\", \\"%s\\", %d, %d)"' %
              (path, outpath, site, phase_for(runno)))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('tag')
    ap.add_argument('-q', '--sockdir',
                    help='Socket dir if using zmq_fan.py to buffer the listfile')
    ap.add_argument('-t', '--timeout', type=float, default=18,
                    help='Timeout when reading directly from listfile (i.e. not using -q)')
    ap.add_argument('-c', '--chunksize', type=int, default=50,
                    help='Chunksize when reading directly from listfile (i.e. not using -q)')
    args = ap.parse_args()

    if args.sockdir:
        reader = ZmqListReader(args.sockdir)
        logger = ZmqListWriter(args.sockdir)
    else:
        fname = input_fname('stage1', args.tag)
        reader = LockfileListReader(fname,
                                    chunksize=args.chunksize,
                                    timeout_secs=args.timeout*3600)
        logger = LockfileListWriter(fname + '.done',
                                    chunksize=args.chunksize)

    outdir = data_dir('stage1_fbf', args.tag)

    with logger:
        for path in reader:
            if random.random() < 0.01:
                sysload()
            process(path, outdir)
            logger.log(path)

if __name__ == '__main__':
    main()
