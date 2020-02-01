#!/usr/bin/env python3

# This code expects IBDSEL_SOCKDIR to be set.
# See bash/stage2_job.sh

import os, argparse, random

from prod_util import worker_timeout_mins, phase_for_day, sysload
from prod_util import stage1_dbd_path, stage2_dbd_path, configfile_path
from zmq_fan import ZmqListReader, ZmqListWriter

def process(site, day, tag, config):
    inpath = stage1_dbd_path(site, day, tag)
    outpath = stage2_dbd_path(site, day, tag, config)
    configpath = configfile_path(tag, config)
    os.system('mkdir -p %s' % os.path.dirname(outpath))
    os.system(('time root -l -b -q "cling/run_stage2.C(\\"%s\\", \\"%s\\", \\"%s\\", ' +
               '%d, %d, %d)"') %
              (configpath, inpath, outpath, site, phase_for_day(day), day))

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('tag')
    ap.add_argument('config')
    args = ap.parse_args()

    reader = ZmqListReader(timeout_mins=worker_timeout_mins())
    logger = ZmqListWriter()

    with logger:
        for line in reader:
            if random.random() < 0.01:
                sysload()
            site, day = map(int, line.split())
            process(site, day, args.tag, args.config)
            logger.log(line)

if __name__ == '__main__':
    main()
