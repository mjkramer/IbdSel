#!/usr/bin/env python3

import argparse
import sys

import pandas as pd
import uproot


def read_file(stage2_pbp_dir, nADs, hall):
    path = f'{stage2_pbp_dir}/stage2.pbp.eh{hall}.{nADs}ad.root:results'
    f = uproot.open(path)
    return (f.arrays(['seq', 'site', 'livetime_s'], library="pd")
             .drop_duplicates())


def read_dir(stage2_pbp_dir):
    dfs = [read_file(stage2_pbp_dir, nADs, hall)
           for nADs in [6, 8, 7]
           for hall in [1, 2, 3]]
    return pd.concat(dfs).sort_values(['seq', 'site'])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('stage2_pbp_dir')
    args = ap.parse_args()

    df = read_dir(args.stage2_pbp_dir)
    assert df is not None
    df.to_csv(sys.stdout, sep='\t', header=False, index=False)


if __name__ == '__main__':
    main()
