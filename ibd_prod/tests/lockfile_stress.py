#!/usr/bin/env python3

"Simulation of multiple parallel jobs hammering the lockfile"

import argparse, time, os
from typing import List
from dataclasses import dataclass
from multiprocessing import Pool
from math import log
from random import random
import numpy as np

from prod_io import LockfileListReader, LockfileListWriter
from prod_util import input_fname

class Timer:
    def __init__(self):
        self.t0 = 0
        self.t1 = 0

    def __enter__(self):
        self.t0 = time.time_ns()
        return self

    def __exit__(self, *_exc):
        self.t1 = time.time_ns()

    def __call__(self, *_):
        return self.t1 - self.t0

@dataclass
class Config:
    infile: str
    outfile: str
    chunksize: int

def init(cfg):
    reader = LockfileListReader(cfg.infile, chunksize=cfg.chunksize,
                                timeout_mins=None)
    writer = LockfileListWriter(cfg.outfile, chunksize=cfg.chunksize)
    return reader, writer

def hammer(cfg):
    "Return time in milliseconds for the pull and flush"

    if hammer.reader is None:
        hammer.reader, hammer.writer = init(cfg)
        hammer.pid = os.getpid()

    print(f'\x1b[7;30;44mStarting {hammer.pid}\x1b[0m')

    r, w = hammer.reader, hammer.writer
    timer = Timer()

    with timer:
        r._pull()
    t_pull = 1e-6 * timer()

    w._buf = r._working
    if not w._buf:
        return None

    with timer:
        w._flush()
    t_flush = 1e-6 * timer()

    print(f'\x1b[7;30;45mEnding {hammer.pid}: {t_pull:.2f} / {t_flush:.2f} ms\x1b[0m')
    return t_pull, t_flush

hammer.reader = None

def t2next(tau):
    p = random()
    return tau * -log(p)   # quantile of expo distribution

def main():
    ap = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    ap.add_argument('tag')
    ap.add_argument('-t', '--timeout-mins', help=' ', type=int, default=0)
    ap.add_argument('-T', '--tau-secs', help='avg time between hammers', type=float, default=5)
    ap.add_argument('-c', '--chunksize', help=' ', type=int, default=500)
    args = ap.parse_args()

    infile = input_fname('stage1', args.tag)
    outfile = infile + '.done'
    cfg = Config(infile, outfile, args.chunksize)

    results: List[(float, float)] = []
    finished = False

    def callback(result):
        nonlocal finished
        if result is None:
            finished = True
        else:
            t_pull, t_flush = result
            results.append((t_pull, t_flush))

    t0 = time.time()

    with Pool(8) as pool:
        while not finished:
            runtime = time.time() - t0
            if args.timeout_mins and runtime > args.timeout_mins * 60:
                break

            sleeptime = t2next(args.tau_secs)
            print(f'\x1b[7;30;42mSleeping for {sleeptime:.2f} s\x1b[0m')
            time.sleep(sleeptime)

            pool.apply_async(hammer, (cfg,), callback=callback)

        pool.close()
        pool.join()

    np.savetxt('stress.txt', results, fmt='%.3f %.3f')

if __name__ == '__main__':
    main()
