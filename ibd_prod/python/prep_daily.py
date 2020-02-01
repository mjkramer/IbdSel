#!/usr/bin/env python3

# Intended to be invoked by scripts/prep_daily.sh.

import argparse
import numpy as np

from daily_runlist import DailyRunList
from prod_util import input_fname

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('step')
    ap.add_argument('tag')
    args = ap.parse_args()

    drl = DailyRunList()
    vals = drl.df[['site', 'day']].sort_values(by=['day', 'site']) \
                                  .drop_duplicates().values
    np.savetxt(input_fname(args.step, args.tag), vals, fmt='%d %d')

if __name__ == '__main__':
    main()
