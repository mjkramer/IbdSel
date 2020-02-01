#!/usr/bin/env python3

import sys
import numpy as np

from daily_runlist import DailyRunList

def main():
    drl = DailyRunList()
    vals = drl.df[['site', 'day']].sort_values(by=['day', 'site']) \
                                  .drop_duplicates().values
    try:
        np.savetxt(sys.stdout.buffer, vals, fmt='%d %d')
    except BrokenPipeError:
        # This can happen when stdout is piped to e.g. the "head" command
        # We close stderr to avoid an "exception ignored" message
        sys.stderr.close()

if __name__ == '__main__':
    main()
