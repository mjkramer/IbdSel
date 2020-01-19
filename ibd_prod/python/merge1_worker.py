#!/usr/bin/env python3

# XXX check f-strings

import os, sys, re, argparse, random
from tempfile import TemporaryDirectory
import ROOT as R
R.PyConfig.IgnoreCommandLineOptions = True

from daily_runlist import DailyRunList
from prod_util import ParallelListReader, DoneLogger, parse_path, sysload, \
    gen2list, chunk_list, log_time, dets_for

JOB_CHUNKSIZE = 10
HADD_CHUNKSIZE = 50

class Merger:
    def __init__(self, cmd_args):
        self.cmd_args = cmd_args
        self.runlist = DailyRunList()

    def input_path(self, site, runno, fileno):
        subdir = runno // 100 * 100
        return os.path.join(self.cmd_args.datadir, 'stage1_fbf', self.cmd_args.tag,
                            f'EH{site}', f'{subdir:07d}', f'{runno:07d}'
                            f'stage1.fbf.eh{site}.{runno:07d}.{fileno:04d}.root')

    def output_path(self, site, day):
        return os.path.join(self.cmd_args.datadir, 'stage1_dbd', self.cmd_args.tag,
                            f'EH{site}',
                            f'stage1.dbd.eh{site}.{day:04d}.root')

    @staticmethod
    def things2check(site, runno):
        return ['h_livetime', 'muons',
                *[f'physics_AD{d}' for d in dets_for(site, runno)]]

    @staticmethod
    def is_readable(path, site, runno):
        things = things2check(site, runno)
        try:
            f = R.TFile(path)
            for thing in things:
                if not f.Get(thing):
                    return False
        except:
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

    @staticmethod
    def hadd(paths, outpath):
        inputs = ' '.join(paths)
        os.system(f'hadd -f {outpath} {inputs}')

    @staticmethod
    def hadd_chunked(paths, outpath):
        chunks = chunk_list(paths, HADD_CHUNKSIZE)

        with TemporaryDirectory() as tmpdir:
            def hadd_chunk(i, chunk):
                tmppath = os.path.join(tmpdir, 'tmp.{i}.root')
                self.hadd(chunk, tmppath)
                return tmppath

            tmp_outputs = [hadd_chunk(i, chunk)
                           for i, chunk in enumerate(chunks)]

            os.system('mkdir -p ' + os.path.dirname(outpath))
            self.hadd(tmp_outputs, outpath)

    @log_time
    def merge(self, site, day):
        print('MERGING', site, day)

        paths = self.readable_files(site, day)
        outpath = self.output_path(site, day)
        self.hadd_chunked(paths, outpath)

    def loop(self):
        donelist = re.sub(r'\.txt$', '.done.txt', self.cmd_args.inputlist)

        with DoneLogger(donelist, chunksize=JOB_CHUNKSIZE) as logger:
            for line in ParallelListReader(cmd_args.inputlist, chunksize=JOB_CHUNKSIZE,
                                           timeout_secs=cmd_args.timeout * 3600):
                if random.random() < 0.1:
                    sysload()
                site, day = map(int, line.split()[:2])
                self.merge(site, day)
                logger.log(line)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('inputlist', help='file format: site day')
    ap.add_argument('datadir')
    ap.add_argument('tag')
    ap.add_argument('-t', '--timeout', type=float, default=18, help='hours')
    args = ap.parse_args()

    merger = Merger(args)
    merger.loop()

if __name__ == '__main__':
    main()
