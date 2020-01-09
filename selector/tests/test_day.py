#!/usr/bin/env python3

import ROOT as R
import os

# For FileFinder
R.gSystem.Load("/usr/common/software/python/2.7-anaconda-2019.07/lib/libsqlite3.so")

for line in ['.x LoadBoost.C',
             '.L tests/FileFinder.cc+',
             ".L stage1_main.cc+"]:
    R.gROOT.ProcessLine(line)

R.Stage, R.Site                 # preload

FF = R.FileFinder()

def process1(fileno):
    outpath = "tests/out.test_day/stage1.%04d.root" % fileno
    inpath = FF.find(21221, fileno)
    R.stage1_main(inpath, outpath, R.k6AD, R.EH1)

def main():
    for fileno in range(58, 210 + 1):
        print(fileno)
        process1(fileno)

    cmd = "hadd -f tests/out_stage1.root tests/out.test_day/stage1.*.root"
    os.system(cmd)

    cmd = "root -b -q -l 'tests/test_stage2.C(1, 1)'"
    os.system(cmd)

if __name__ == '__main__':
    main()
