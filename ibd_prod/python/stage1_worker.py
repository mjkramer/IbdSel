#!/usr/bin/env python3

# This code expects the environment variables exported in
# bash/stage1_vars.inc.sh. See bash/stage1_job.sh

import os, argparse, random

from prod_util import parse_path, sysload, phase_for, data_dir
from prod_util import worker_timeout_mins
from zmq_fan import ZmqListReader, ZmqListWriter

def process(path, tag):
    outdir = data_dir('stage1_fbf', tag)
    runno, fileno, site = parse_path(path)
    subdir = runno // 100 * 100
    outpath = os.path.join(outdir, 'EH%d' % site, '%07d' % subdir, '%07d' % runno,
                           'stage1.fbf.eh%d.%07d.%04d.root' % (site, runno, fileno))
    os.system('mkdir -p %s' % os.path.dirname(outpath))
    os.system('time root -l -b -q "cling/run_stage1.C(\\"%s\\", \\"%s\\", %d, %d)"' %
              (path, outpath, site, phase_for(runno)))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('sockdir', help='Dir containing zmq_fan sockets')
    ap.add_argument('tag')
    args = ap.parse_args()

    reader = ZmqListReader(args.sockdir, timeout_mins=worker_timeout_mins())
    logger = ZmqListWriter(args.sockdir)

    with logger:
        for path in reader:
            if random.random() < 0.01:
                sysload()
            process(path, args.tag)
            logger.log(path)

if __name__ == '__main__':
    main()
