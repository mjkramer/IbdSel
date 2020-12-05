#!/usr/bin/env python3

import argparse
import datetime as D
import gzip
import os

import pandas as pd

DAY_ZERO = D.datetime(2011, 12, 24)


def parse_path(path):
    "Returns (runno, fileno, site)"
    base = path.strip().split("/")[-1]
    parts = base.split(".")
    return int(parts[2]), int(parts[6][1:]), int(parts[4][2])


def get_days(firstrun, lastrun):
    passwd = os.environ["DBPASS"]
    con = f"mysql://dayabay:{passwd}@dybdb1.ihep.ac.cn/offline_db"
    query = f"""SELECT runno, fileno, timeend FROM DaqRawDataFileInfo
                NATURAL JOIN DaqRawDataFileInfoVld
                WHERE stream LIKE 'EH_-Merged' AND streamtype = 'Physics'
                AND runno between {firstrun} and {lastrun}"""
    df = pd.read_sql(query, con).set_index(['runno', 'fileno'])
    return df.apply(axis=1, func=lambda row: (row.timeend - DAY_ZERO).days)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("listfile",
                    help="List of input DAQ files (e.g. for stage1)")
    args = ap.parse_args()

    if not os.environ["DBPASS"]:
        print("You must set DBPASS!")
        return

    listfile = gzip.open(args.listfile) if args.listfile.endswith("gz") \
        else open(args.listfile)

    files = {parse_path(line) for line in listfile}

    firstrun = min(f[0] for f in files)
    lastrun = max(f[0] for f in files)
    days = get_days(firstrun, lastrun)

    for runno, fileno, site in sorted(files):
        print(days[runno, fileno], site, runno, fileno)


if __name__ == '__main__':
    main()
