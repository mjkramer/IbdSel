#!/usr/bin/env python

from collections import defaultdict
from itertools import product

class HistAccum(object):
    def __init__(self):
        self._h = None

    def __iadd__(self, h2add):
        if self._h is None:
            self._h = h2add.Clone()
        else:
            self._h.Add(h2add)

    def __call__(self):
        return self._h

def main_for(period):
    ftxt, fprompt, facc = output_files(period)

    ftxt.startStamp = int(1e13)
    ftxt.endStamp = 0

    wsums = defaultdict(lambda: [0.]*8)
    wsumsErr = defaultdict(lambda: [0.]*8)

    hpromptAccums = [HistAccum() for _ in range(8)]
    haccAccums = [HistAccum() for _ in range(8)]

    weeks = weeks_of(period)
    for week, site in product(weeks, [1, 2, 3]):
        f = input_file(period, week, site)

        ftxt.startStamp = min(f.firstTrigTime(), ftxt.startStamp)
        ftxt.endStamp = max(f.lastTrigTime(), ftxt.endStamp)

        for AD in ADs_of(site):
            def inc_wsum(name):
                wsums[name][AD] += \
                    f.livetime(AD) * getattr(f, name)(AD)

            def inc_wsum2(name):
                inc_wsum(name)
                wsumsErr[name][AD] += \
                    (f.livetime(AD) * getattr(f, name+'Err')(AD)) ** 2

            ftxt.nobs[AD] += f.nobs(AD)
            ftxt.livetime[AD] += f.livetime(AD)

            for eff in ['mu', 'dmc']:
                inc_wsum(eff + 'Eff')

            for bkg in ['Tot', 'acc', 'li9', 'fn', 'amc', 'aln']:
                inc_wsum2(bkg + 'Bkg')

            hpromptAccums[AD] += f.hprompt(AD)
            haccAccums[AD] += f.hacc(AD)

    for AD in range(8):
        for name, tot in wsums.items():
            getattr(ftxt, name)[AD] = tot / ftxt.livetime[AD]
        for name, tot in wsumsErr.items():
            getattr(ftxt, name + 'Err')[AD] = sqrt(tot) / ftxt.livetime[AD]

    ftxt.write()
    fprompt.write([ha() for ha in hpromptAccums])
    facc.write([ha() for ha in haccAccums])

def main():
    for period in range(NPERIODS):
        main_for(period)
