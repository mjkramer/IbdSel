#!/usr/bin/env python3

import argparse
import os

import ROOT as R


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('confFile')
    ap.add_argument('stage2File')
    ap.add_argument('site')
    ap.add_argument('phase')
    args = ap.parse_args()

    exe = "$IBDSEL_HOME/_build/recalc.exe"
    cmd = f"{exe} {args.confFile} {args.stage2File}" + \
        f" {args.site} {args.phase}"
    os.system(cmd)

    f = R.TFile(args.stage2File, "UPDATE")
    f.Delete("results")
    tree = f.Get("results_recalc")
    tree.SetName("results")
    tree.Write()


if __name__ == "__main__":
    main()
