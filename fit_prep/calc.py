"Calculate the things"

from hardcoded import Hardcoded
from delayed_eff import DelayedEffCalc

from prod_util import dets_for_phase, idet

from root_pandas import read_root
from numpy import sqrt
import ROOT as R

import os

class Calc:
    "Calculate for a given phase"

    def __init__(self, phase, stage2_dir, config_path):
        self.phase = phase
        self.hardcoded = Hardcoded(phase)

        self.files, self.results = {}, {}
        for site in [1, 2, 3]:
            nADs = [6, 8, 7][phase-1]
            fname = f'stage2.pbp.eh{site}.{nADs}ad.root'
            path = os.path.join(stage2_dir, fname)

            self.files[site] = R.TFile(path)

            results = read_root(path, 'results')
            for det in dets_for_phase(site, phase):
                self.results[(site, det)] = \
                    results.query(f'detector == {det}')

        self.delEffCalc = DelayedEffCalc(config_path)

    def _livetime_weighted(self, site, det, var):
        r = self.results[(site, det)]
        return (r[var] * r.livetime_s).sum() / r.livetime_s.sum()

    def _livetime_weighted_squared(self, site, det, var):
        r = self.results[(site, det)]
        return sqrt((r[var]**2 * r.livetime_s).sum()) / r.livetime_s.sum()

    def _relDelEff(self, site, det):
        return self.delEffCalc.scale_factor(self.phase, site, det)

    def ibdCount(self, site, det):
        tree = self.files[site].Get(f'ibd_AD{det}')
        return tree.GetEntries()

    def livetime(self, site, det):
        sec = self.results[(site, det)]['livetime_s'].sum()
        return sec / (3600 * 24)

    def vetoEff(self, site, det):
        return self._livetime_weighted(site, det, 'vetoEff')

    def dmcEff(self, site, det):
        return self._livetime_weighted(site, det, 'dmcEff')

    def accBkg(self, site, det):
        return self._livetime_weighted(site, det, 'accDaily')

    def accBkgErr(self, site, det):
        # return self._livetime_weighted_squared(site, det, 'accDailyErr')
        return 0.01 * self.accBkg(site, det)

    def li9Bkg(self, site, det):
        scale = self._relDelEff(site, det)
        return scale * self._livetime_weighted(site, det, 'li9Daily')

    def li9BkgErr(self, site, det):
        scale = self._relDelEff(site, det)
        return scale * self._livetime_weighted(site, det, 'li9DailyErr')

    def totalBkg(self, site, det):
        return (self.accBkg(site, det) +
                self.li9Bkg(site, det) +
                self.fastnBkg(site, det) +
                self.amcBkg(site, det) +
                self.alphanBkg(site, det))

    def totalBkgErr(self, site, det):
        return sqrt(self.accBkgErr(site, det)**2 +
                    self.li9BkgErr(site, det)**2 +
                    self.fastnBkgErr(site, det)**2 +
                    self.amcBkgErr(site, det)**2 +
                    self.alphanBkgErr(site, det)**2)

    def fastnBkg(self, site, det):
        scale = self._relDelEff(site, det)
        return scale * self._hardcoded(site, det, 'fastnBkg')

    def fastnBkgErr(self, site, det):
        scale = self._relDelEff(site, det)
        return scale * self._hardcoded(site, det, 'fastnBkgErr')

    # TODO scale for modified delayed cut
    def amcBkg(self, site, det):
        return self._hardcoded(site, det, 'amcBkg')

    # TODO scale for modified delayed cut
    def amcBkgErr(self, site, det):
        return self._hardcoded(site, det, 'amcBkgErr')

    def alphanBkg(self, site, det):
        scale = self._relDelEff(site, det)
        return scale * self._hardcoded(site, det, 'alphanBkg')

    def alphanBkgErr(self, site, det):
        scale = self._relDelEff(site, det)
        return scale * self._hardcoded(site, det, 'alphanBkgErr')

    def targetMass(self, site, det):
        return self._hardcoded(site, det, 'targetMass')

    # unused
    def delayedEff(self, site, det):
        return self._hardcoded(site, det, 'delayedEff')

    # unused
    def powerErr(self, site, det):
        return self._hardcoded(site, det, 'powerErr')

    # unused
    def totEffErr(self, site, det):
        return self._hardcoded(site, det, 'totEffErr')

    # unused?
    def timestamps(self):
        return self.hardcoded.timestamps()

    def _hardcoded(self, site, det, name):
        i = idet(site, det)
        return self.hardcoded.__getattribute__(name)()[i]
