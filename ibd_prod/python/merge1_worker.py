#!/usr/bin/env python3

import os, argparse, random
import ROOT as R
R.PyConfig.IgnoreCommandLineOptions = True

from daily_runlist import DailyRunList
from prod_util import sysload, gen2list, log_time, dets_for, input_fname, data_dir
from prod_io import LockfileListReader, LockfileListWriter
from hadd import hadd_chunked

JOB_CHUNKSIZE = 10
HADD_CHUNKSIZE = 50

class Merger:
    def __init__(self, cmd_args):
        self.cmd_args = cmd_args
        self.runlist = DailyRunList()

    def input_path(self, site, runno, fileno):
        subdir = runno // 100 * 100
        return os.path.join(data_dir('stage1_fbf', self.cmd_args.tag),
                            f'EH{site}', f'{subdir:07d}', f'{runno:07d}'
                            f'stage1.fbf.eh{site}.{runno:07d}.{fileno:04d}.root')

    def output_path(self, site, day):
        return os.path.join(data_dir('stage1_dbd', self.cmd_args.tag),
                            f'EH{site}',
                            f'stage1.dbd.eh{site}.{day:04d}.root')

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
            path = self.input_path(site, runno, fileno)
            if self.is_readable(path, site, runno):
                yield path
            else:
                print(f'CRAPPY {site} {day} {runno} {path}')

    @log_time
    def merge(self, site, day):
        print(f'MERGING {site} {day}')

        paths = self.readable_files(site, day)
        outpath = self.output_path(site, day)
        hadd_chunked(paths, outpath, HADD_CHUNKSIZE)

    def input_fname(self):
        return input_fname('merge1', self.cmd_args.tag)

    def loop(self):
        reader = LockfileListReader(self.input_fname(),
                                    chunksize=JOB_CHUNKSIZE,
                                    timeout_secs=self.cmd_args.timeout * 3600)
        logger = LockfileListWriter(self.input_fname() + '.done',
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
    ap.add_argument('-t', '--timeout', type=float, default=18, help='hours')
    args = ap.parse_args()

    merger = Merger(args)
    merger.loop()

if __name__ == '__main__':
    main()
