#!/usr/bin/env python2.7

# Recommend using 50
CHUNKSIZE = 50   # how many input files to pop off the list at a time (default)

import os, time, argparse
from common import LockfileListReader, DoneLogger, parse_path

def process(path, outdir):
    runno, fileno, site = parse_path(path)
    subdir = runno / 100 * 100
    outpath = os.path.join(outdir, 'EH%d' % site, '%07d' % subdir,
                           'pre_ibd.%07d.%04d.root' % (runno, fileno))
    os.system('mkdir -p %s' % outpath)  # this actually works because ROOT will delete empty dir when open TFile w/ "RECREATE" lol
    os.system('time root -b -q "../selector/stage1_main.cc+(\\"%s\\", \\"%s\\", %d)"' %
              (path, outpath, site))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('inputlist')
    ap.add_argument('outputdir')
    ap.add_argument('-t', '--timeout-mins', type=float, default=90)
    ap.add_argument('-c', '--chunksize', type=int, default=CHUNKSIZE)
    args = ap.parse_args()

    donelist = args.inputlist + '.done'

    with DoneLogger(donelist, chunksize=args.chunksize) as logger:
        for path in LockfileListReader(args.inputlist, chunksize=args.chunksize,
                                       timeout_mins = args.timeout_mins):
            # sysload()
            process(path, args.outputdir)
            logger.log(path)

if __name__ == '__main__':
    main()
