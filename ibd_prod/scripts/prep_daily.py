#!/usr/bin/env python3

import argparse
import numpy as np

from daily_runlist import DailyRunList

ap = argparse.ArgumentParser()
ap.add_argument('category')
ap.add_argument('tag')
args = ap.parse_args()

outfile = f'../../data/{args.category}_input/{args.tag}/input.{args.category}.txt'

drl = DailyRunList()
vals = drl.df[['site', 'day']].drop_duplicates().values

np.savetxt(outfile, vals, fmt='%d %d')
