"Calculate the things"

from config_file import ConfigFile
from hardcoded import Hardcoded

from prod_util import dets_for_phase, idet

from root_pandas import read_root
from numpy import sqrt
import ROOT as R

import os

# XXX delete me when deleting Calc.conf etc. (see below)
from util import load_selector
load_selector()


# XXX If Li9Calc is working from within stage2 again, we can get rid of
# self.conf and self.li9calc and just use the commented versions of li9Bkg[Err]
class Calc:
    "Calculate for a given phase"

    def __init__(self, phase, stage2_dir, config_path):
        self.phase = phase
        self.hardcoded = Hardcoded(phase)

        # XXX
        self.conf = ConfigFile(config_path)
        self.li9calc = R.Li9Calc()

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

    def _livetime_weighted(self, site, det, var):
        r = self.results[(site, det)]
        return (r[var] * r.livetime_s).sum() / r.livetime_s.sum()

    def _livetime_weighted_squared(self, site, det, var):
        r = self.results[(site, det)]
        return sqrt((r[var]**2 * r.livetime_s).sum()) / r.livetime_s.sum()

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

    # XXX
    # def li9Bkg(self, site, det):
    #     return self._livetime_weighted(site, det, 'li9Bkg')
    #
    # def li9BkgErr(self, site, det):
    #     return self._livetime_weighted(site, det, 'li9BkgErr')
    def li9Bkg(self, site, _det):
        shower_pe = self.conf['showerMuChgCut']
        showerVeto_ms = 1e-3 * self.conf['showerMuPostVeto_us']
        return self.li9calc.li9daily(site, shower_pe, showerVeto_ms)

    def li9BkgErr(self, site, det):
        fracUnc = [0.27, 0.29, 0.37]
        return fracUnc[site-1] * self.li9Bkg(site, det)

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
        return self._hardcoded(site, det, 'fastnBkg')

    def fastnBkgErr(self, site, det):
        return self._hardcoded(site, det, 'fastnBkgErr')

    def amcBkg(self, site, det):
        return self._hardcoded(site, det, 'amcBkg')

    def amcBkgErr(self, site, det):
        return self._hardcoded(site, det, 'amcBkgErr')

    def alphanBkg(self, site, det):
        return self._hardcoded(site, det, 'alphanBkg')

    def alphanBkgErr(self, site, det):
        return self._hardcoded(site, det, 'alphanBkgErr')

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
