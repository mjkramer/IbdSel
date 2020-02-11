#!/usr/bin/env python3

import ROOT as R
import os
import sys

sys.path.insert(0, '../ibd_prod/python')
from daily_runlist import DailyRunList
from prod_util import phase_for_run

# For FileFinder
# R.gSystem.Load("/usr/common/software/python/2.7-anaconda-2019.07/lib/libsqlite3.so")
# No need to do that when using PyROOT since Python is linked to libsqlite3

R.gROOT.ProcessLine('.x cling/Build.C("stage1/stage1_main.cc")')
R.gROOT.ProcessLine('.L tests/FileFinder.cc+')

R.Phase, R.Site                 # preload

FF = R.FileFinder()
DRL = DailyRunList()

def process1(site, runno, fileno):
    outpath = "tests/out.test_day/stage1.%06d.%04d.root" % (runno, fileno)
    inpath = FF.find(int(runno), int(fileno))
    R.stage1_main(inpath, outpath, site, phase_for_run(runno))

def go(site, day):
    os.system('rm -f tests/out.test_day/*')

    for runno, fileno in DRL.daily(site=site, day=day):
        print(runno, fileno)
        process1(site, runno, fileno)

    for cmd in [
            "hadd -f tests/out_stage1.root tests/out.test_day/stage1.*.root",
            "root -b -q -l 'tests/test_stage2.C(%d, %d)'" % (site, phase_for_run(runno)),
            "mv tests/out_stage2.root tests/out_stage2_%d_%04d.root" % (site, day)
    ]:
        os.system(cmd)

    f = R.TFile('tests/out_stage2_%d_%04d.root' % (site, day))
    f.results.Scan()
    f.Close()
