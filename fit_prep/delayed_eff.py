"Calculate the RELATIVE efficiency (vs 6 MeV) of a delayed energy cut"

import os

import ROOT as R

from config_file import ConfigFile
from get_ncap_spec import get_ncap_spec
from vertex_eff import VertexEffCalc


class DelayedEffCalc:
    def __init__(self, config_path, calc):
        config = ConfigFile(config_path)
        self.cut = config["ibdDelayedEmin"]
        self.vtxcut = VertexEffCalc.load_cut(config_path)
        self.calc = calc

    def scale_factor(self, phase, site, det):
        h = get_ncap_spec(phase, site, det, self.calc, self.vtxcut)

        nom = h.Integral(h.FindBin(self.cut), h.FindBin(12))
        denom = h.Integral(h.FindBin(6), h.FindBin(12))

        return nom / denom
