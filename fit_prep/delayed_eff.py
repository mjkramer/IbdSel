"Calculate the RELATIVE efficiency (vs 6 MeV) of a delayed energy cut"

import os

import ROOT as R

from config_file import ConfigFile


class DelayedEffCalc:
    def __init__(self, config_path):
        config = ConfigFile(config_path)
        self.cut = config["ibdDelayedEmin"]

        home = os.getenv("IBDSEL_HOME")
        ncap_spec_path = f"{home}/static/ncap_spec/ncap_spec_P17B.root"
        self.specfile = R.TFile(ncap_spec_path)

    def scale_factor(self, phase, site, det):
        nADs = [6, 8, 7][phase-1]
        hname = f"h_ncap_{nADs}ad_eh{site}_ad{det}"
        h = self.specfile.Get(hname)

        nom = h.Integral(h.FindBin(self.cut), h.FindBin(12))
        denom = h.Integral(h.FindBin(6), h.FindBin(12))

        return nom / denom
