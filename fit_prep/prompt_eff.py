"""
Calculate the RELATIVE efficiency (vs 0.7 MeV), for correlated backgrounds, for
a given prompt energy cut
"""

from functools import lru_cache
import os

import ROOT as R

from prod_util import idet as get_idet

from config_file import ConfigFile


class PromptEffCalc:
    def __init__(self, config_path):
        config = ConfigFile(config_path)
        self.emin = config["ibdPromptEmin"]

    @lru_cache()
    def li9_rel_eff(self, _site, _det):
        home = self._fit_home()
        path = f"{home}/li9_spectrum/8he9li_nominal_spectrum.root"
        f = R.TFile(path)

        h = f.h_nominal
        return self._hist_rel_eff(h)

    @lru_cache()
    def fastn_rel_eff(self, site, det, ihep=False):
        home = self._fit_home()
        suffix = "_IHEP" if ihep else ""
        path = f"{home}/fn_spectrum/P15A_fn_spectrum{suffix}.root"
        f = R.TFile(path)

        detno = 1 + get_idet(site, det)
        h = f.Get(f"h_{detno}AD_fn_fine")
        return self._hist_rel_eff(h)

    @lru_cache()
    def amc_rel_eff(self, _site, _det):
        home = self._fit_home()
        path = f"{home}/amc_spectrum/amc_spectrum.root"
        f = R.TFile(path)

        fn = f.expo
        return self._func_rel_eff(fn)

    @lru_cache()
    def alphan_rel_eff(self, site, det):
        home = self._fit_home()
        path = f"{home}/alpha-n-spectrum/result-DocDB9667.root"
        f = R.TFile(path)

        detno = 1 + get_idet(site, det)
        h = f.Get(f"AD{detno}")
        return self._hist_rel_eff(h)

    @staticmethod
    def _fit_home():
        return os.environ["LBNL_FIT_HOME"]

    def _hist_rel_eff(self, h):
        igral = h.Integral(h.FindBin(self.emin), h.FindBin(12))
        igral_nom = h.Integral(h.FindBin(0.7), h.FindBin(12))
        return igral / igral_nom

    def _func_rel_eff(self, fn):
        igral = fn.Integral(self.emin, 12)
        igral_nom = fn.Integral(0.7, 12)
        return igral / igral_nom
