#!/usr/bin/env python3

import os, re, argparse, random
from prod_util import ParallelListReader, DoneLogger, parse_path, sysload, stage_for
from queue_buffer import BufferedParallelListReader, BufferedDoneLogger

def process(path, outdir):
    runno, fileno, site = parse_path(path)
    subdir = runno // 100 * 100
    outpath = os.path.join(outdir, 'EH%d' % site, '%07d' % subdir, '%07d' % runno,
                           'stage1.fbf.eh%d.%07d.%04d.root' % (site, runno, fileno))
    os.system('mkdir -p %s' % os.path.dirname(outpath))
    os.system('time root -l -b -q "cling/run_stage1.C(\\"%s\\", \\"%s\\", %d, %d)"' %
              (path, outpath, site, stage_for(runno)))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('inputlist')
    ap.add_argument('outputdir')
    ap.add_argument('-q', '--sockdir',
                    help='Socket dir if using queue_buffer.py to buffer the listfile')
    ap.add_argument('-t', '--timeout', type=float, default=18,
                    help='Timeout when reading directly from listfile (i.e. not using -q)')
    ap.add_argument('-c', '--chunksize', type=int, default=50,
                    help='Chunksize when reading directly from listfile (i.e. not using -q)')
    args = ap.parse_args()

    if args.sockdir:
        getPLR = lambda: BufferedParallelListReader(args.sockdir)
        getDL = lambda: BufferedDoneLogger(args.sockdir)
    else:
        donelist = re.sub(r'\.txt$', '.done.txt', args.inputlist)
        getPLR = lambda: ParallelListReader(args.inputlist, chunksize=args.chunksize,
                                            timeout_secs=args.timeout*3600)
        getDL = lambda: DoneLogger(donelist, chunksize=args.chunksize)

    with getDL() as logger:
        for path in getPLR():
            if random.random() < 0.01:
                sysload()
            process(path, args.outputdir)
            logger.log(path)

if __name__ == '__main__':
    main()
