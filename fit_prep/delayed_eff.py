"Calculate the RELATIVE efficiency (vs 6 MeV) of a delayed energy cut"

# NOTE: We have retired this in favor of the calculation in selector's
# Calculator. To revive this, make sure that "calc" refers to the same
# tag@config used by get_ncap_spec. Ideally, ncap spec is measured using same
# vertex cut as we are using.

import os

import ROOT as R

from config_file import ConfigFile
from get_ncap_spec import get_ncap_spec
# from vertex_eff import VertexEffCalc


class DelayedEffCalc:
    def __init__(self, config_path, calc):
        config = ConfigFile(config_path)
        self.cut = config["ibdDelayedEmin"]
        # self.vtxcut = VertexEffCalc.load_cut(config_path)
        self.calc = calc

    def scale_factor(self, phase, site, det):
        # h = get_ncap_spec(phase, site, det, self.calc, self.vtxcut)
        h = get_ncap_spec(phase, site, det, self.calc)

        nom = h.Integral(h.FindBin(self.cut), h.FindBin(12))
        denom = h.Integral(h.FindBin(6), h.FindBin(12))

        return nom / denom
