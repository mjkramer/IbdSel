#!/usr/bin/env python3

import argparse, random
import ROOT as R
R.PyConfig.IgnoreCommandLineOptions = True

from daily_runlist import DailyRunList
from prod_util import gen2list, log_time, dets_for, input_fname
from prod_util import stage1_fbf_path, stage1_dbd_path
from prod_util import unbuf_stdout, sysload
from prod_io import LockfileListReader, LockfileListWriter
from hadd import hadd_chunked

JOB_CHUNKSIZE = 10
HADD_CHUNKSIZE = 50

class Merger:
    def __init__(self, cmd_args):
        self.cmd_args = cmd_args
        self.runlist = DailyRunList()

    @staticmethod
    def things2check(site, runno):
        return ['h_livetime', 'muons',
                *[f'physics_AD{d}' for d in dets_for(site, runno)]]

    @staticmethod
    def is_readable(path, site, runno):
        try:
            f = R.TFile(path)
            for thing in Merger.things2check(site, runno):
                if not f.Get(thing):
                    return False
        except Exception:       # pylint: disable=broad-except
            return False
        return True

    @gen2list
    def readable_files(self, site, day):
        for runno, fileno in self.runlist.daily(site=site, day=day):
            path = stage1_fbf_path(site, runno, fileno, self.cmd_args.tag)
            if self.is_readable(path, site, runno):
                yield path
            else:
                print(f'CRAPPY {site} {day} {runno} {path}')

    @log_time
    def merge(self, site, day):
        print(f'MERGING {site} {day}')

        paths = self.readable_files(site, day)
        outpath = stage1_dbd_path(site, day, self.cmd_args.tag)
        hadd_chunked(paths, outpath, HADD_CHUNKSIZE)

    def loop(self):
        listfile = input_fname('merge1', self.cmd_args.tag)
        reader = LockfileListReader(listfile,
                                    chunksize=JOB_CHUNKSIZE,
                                    timeout_mins=self.cmd_args.timeout_mins)
        logger = LockfileListWriter(listfile + '.done',
                                    chunksize=JOB_CHUNKSIZE)

        with logger:
            for line in reader:
                if random.random() < 0.1:
                    sysload()
                site, day = map(int, line.split()[:2])
                self.merge(site, day)
                logger.log(line)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('tag')
    ap.add_argument('-t', '--timeout-mins', type=float, default=0)
    args = ap.parse_args()

    merger = Merger(args)
    merger.loop()

if __name__ == '__main__':
    unbuf_stdout()
    main()
