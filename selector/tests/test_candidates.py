#!/usr/bin/env python3

# Run me from ..

from __future__ import print_function

import os
from subprocess import check_output
import pandas as pd
import ROOT as R

CANDIDIR = "/global/project/projectdirs/dayabay/scratch/mkramer/p17b/data_ana_lbnl/output/v3.noSCNL/candidates/lists/AdSimple"

DEBUG_MODE = os.getenv("IBDSEL_DEBUG") == "1"

if DEBUG_MODE:
    print("debug mode")

R.gSystem.Load("/usr/common/software/python/2.7-anaconda-2019.07/lib/libsqlite3.so")
R.gROOT.ProcessLine(".L tests/FileFinder.cc+")

# Load just so we have access to enums/constants
R.gROOT.ProcessLine(".L stage1_main.cc+" + ("g" if DEBUG_MODE else ""))
# R.gROOT.ProcessLine(".L stage2_main.cc+" + ("g" if DEBUG_MODE else ""))

# force pyroot to load R.k6AD etc.
R.Stage

# sigh: https://sft.its.cern.ch/jira/browse/ROOT-7240
def stage_for(runno):
    if 21221 <= runno <= 26694:
        return R.k6AD
    if 34523 <= runno <= 67012:
        return R.k8AD
    if 67625 <= runno <= 72455:
        return R.k7AD
    raise "Nonsensical run number"

def site_for(filename):
    return int(filename.split('/')[-1].split('.')[4][2])

def candis_df(site):
    return pd.read_csv(os.path.join(CANDIDIR, "EH%d.txt" % site), sep=r'\s+')

def parse_stage2_output(line):
    words = line.split()
    assert(len(words) == 4 and words[0] == "IBD" and words[1].startswith('AD'))
    det = int(words[1][2])
    trigP, trigD = int(words[2]), int(words[3])
    return det, trigP, trigD

def run_stage1(runno, fileno, stage, site):
    os.system("root -l -q 'tests/test_stage1.C(%d, %d, %d, %d)'" %
              (runno, fileno, stage, site))

def run_stage2(stage, site):
    output = check_output("root -l -q 'tests/test_stage2.C(%d, %d)'" % (stage, site),
                          shell=True)
    return [parse_stage2_output(line) for line in output.decode().splitlines()
            if line.startswith('IBD AD')]

def run_all(runno, fileno, stage, site):
    run_stage1(runno, fileno, stage, site)
    return run_stage2(stage, site)

class Runner(object):
    def __init__(self):
        self.candis = {site: candis_df(site) for site in [1, 2, 3]}
        self.ff = R.FileFinder()

    def compare(self, runno, fileno):
        fname = self.ff.find(runno, fileno)
        site = site_for(fname)
        stage = stage_for(runno)

        candis = self.candis[site]
        cond = (candis.RunNo == runno) & (candis.FileNo == fileno)
        expected = [(_.Detector, _.trigno_prompt, _.trigno_delayed)
                    for _ in candis.loc[cond].itertuples()]

        actual = run_all(runno, fileno, stage, site)

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
