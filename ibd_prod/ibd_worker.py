#!/usr/bin/env python2.7

# Recommend using 50
CHUNKSIZE = 50              # how many input files to pop off the list at a time

import os, time, re, argparse
from common import ParallelListReader, DoneLogger, parse_path

def process(path, outdir):
    runno, fileno, site = parse_path(path)
    subdir = runno / 100 * 100
    outpath = os.path.join(outdir, 'EH%d' % site, '%07d' % subdir,
                           'pre_ibd.%07d.%04d.root' % (runno, fileno))
    os.system('mkdir -p %s' % outpath)
    os.system('time root -b -q "../selector/stage1_main.cc+(\\"%s\\", \\"%s\\", %d)"' %
              (path, outpath, site))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('inputlist')
    ap.add_argument('outputdir')
    ap.add_argument('-t', '--timeout', type=float, default=18, help='hours')
    args = ap.parse_args()

    donelist = re.sub(r'\.txt$', '.done.txt', args.inputlist)

    with DoneLogger(donelist, chunksize=CHUNKSIZE) as logger:
        for path in ParallelListReader(args.inputlist, chunksize=CHUNKSIZE,
                                       timeout_secs = args.timeout * 3600):
            # sysload()
            process(path, args.outputdir)
            logger.log(path)

if __name__ == '__main__':
    main()
