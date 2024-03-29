"Calculate the RELATIVE efficiency (vs 6 MeV) of a delayed energy cut"

# NOTE: We have retired this in favor of the calculation in selector's
# Calculator. To revive this, make sure that "calc" refers to the same
# tag@config used by get_ncap_spec. Ideally, ncap spec is measured using same
# vertex cut as we are using.

# NOTE: Now this isn't fully retired. It's what we use in fit_prep using the
# "old" implementation. Also this is what we used for the ToyMC studies.

import ROOT as R

import calc
from get_ncap_spec import get_ncap_spec
# from vertex_eff import VertexEffCalc


class DelayedEffCalc:
    def __init__(self, cut, phase, ref_tag, ref_config):
        self.cut = cut
        # self.vtxcut = VertexEffCalc.load_cut(config_path)
        self.calc = calc.Calc(phase, ref_tag, ref_config)
        self.ref_tag = ref_tag
        self.ref_config = ref_config

    def scale_factor(self, site, det, ref_emin):
        # h = get_ncap_spec(phase, site, det, self.calc, self.vtxcut)
        h = get_ncap_spec(self.calc, site, det)

        nom = R.fine_integral(h, self.cut, 12)
        denom = R.fine_integral(h, ref_emin, 12)

        return nom / denom
