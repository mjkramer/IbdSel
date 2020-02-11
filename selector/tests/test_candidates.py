#!/usr/bin/env python3

# Run me from ..

import os
from subprocess import check_output
import pandas as pd
import ROOT as R
from root_pandas import read_root

sys.path.insert(0, '../ibd_prod/python')
from prod_util import phase_for_run

R.gErrorIgnoreLevel = R.kError    # suppress warnings of empty trees

CANDIDIR = "/global/project/projectdirs/dayabay/scratch/mkramer/p17b/data_ana_lbnl/output/v3.noSCNL/candidates/lists/AdSimple"

DEBUG_MODE = os.getenv("IBDSEL_DEBUG") == "1"

if DEBUG_MODE:
    print("debug mode")

R.gSystem.Load("/usr/common/software/python/2.7-anaconda-2019.07/lib/libsqlite3.so")
R.gROOT.ProcessLine(".L tests/FileFinder.cc+")

# Load just so we have access to enums/constants, util::ADsFor, etc.
R.gROOT.ProcessLine(".L stage1/stage1_main.cc+" + ("g" if DEBUG_MODE else ""))
# R.gROOT.ProcessLine(".L stage2_main.cc+" + ("g" if DEBUG_MODE else ""))

# force pyroot to load R.k6AD etc.
R.Phase

def site_for(filename):
    return int(filename.split('/')[-1].split('.')[4][2])

def candis_df(site):
    return pd.read_csv(os.path.join(CANDIDIR, "EH%d.txt" % site), sep=r'\s+')

def read_stage2_output(site, phase):
    results = []
    for det in R.util.ADsFor(site, phase):
        try:
            df = read_root('tests/out_stage2.root', 'ibd_AD%d' % det)
        except OSError:         # root-numpy raises when tree is empty >_<
            continue
        for _ in df.itertuples():
            results.append((det, _.trigP, _.trigD))
    return results

def run_stage1(runno, fileno, site, phase):
    os.system("root -l -q 'tests/test_stage1.C(%d, %d, %d, %d)'" %
              (runno, fileno, site, phase))

def run_stage2(site, phase):
    output = os.system("root -l -q 'tests/test_stage2.C(%d, %d)'" % (site, phase))
    return read_stage2_output(site, phase)

def run_all(runno, fileno, site, phase):
    run_stage1(runno, fileno, site, phase)
    return run_stage2(site, phase)

class Runner(object):
    def __init__(self):
        self.candis = {site: candis_df(site) for site in [1, 2, 3]}
        self.ff = R.FileFinder()

    def compare(self, runno, fileno):
        fname = self.ff.find(runno, fileno)
        site = site_for(fname)
        phase = phase_for_run(runno)

        candis = self.candis[site]
        cond = (candis.RunNo == runno) & (candis.FileNo == fileno)
        expected = [(_.Detector, _.trigno_prompt, _.trigno_delayed)
                    for _ in candis.loc[cond].itertuples()]

        actual = run_all(runno, fileno, site, phase)

        for c in actual:
            det, trigP, trigD = c
            if c not in expected:
                print("EXTRA: AD%d %d %d" % (det, trigP, trigD))
            else:
                print("GOOD: AD%d %d %d" % (det, trigP, trigD))

        for c in expected:
            det, trigP, trigD = c
            if c not in actual:
                print("MISSING: AD%d %d %d" % (det, trigP, trigD))

    def randfile(self):
        allcas = pd.concat([self.candis[i] for i in [1, 2, 3]])
        allrfs = allcas[['RunNo', 'FileNo']].drop_duplicates()

        return [int(_) for _ in allrfs.sample(1).values[0]]

    def compare_rand(self):
        return self.compare(*self.randfile())
