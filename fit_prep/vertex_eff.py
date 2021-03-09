from dataclasses import dataclass
from functools import lru_cache

from config_file import ConfigFile
from prod_util import configfile_path

import calc as _calc


def Calc(*a, **kw):
    return _calc.Calc(*a, **kw)


@dataclass
class VertexCut:
    minZ: float = -10_000
    maxZ: float = 10_000
    minR: float = 0
    maxR: float = 10_000


class VertexEffCalc:
    def __init__(self, owner_calc, phase, tag, config,
                 nom_tag="2021_02_03", nom_config="test_newDelEff_fullDet"):
        self.owner_calc = owner_calc

        for site in [1, 2, 3]:
            self.nom_calc = Calc(phase, nom_tag, nom_config,
                                 self.owner_calc.delayed_eff_mode,
                                 self.owner_calc.delayed_eff_impl)

        cfg_path = configfile_path(tag, config)
        self.cut = self._load_cut(cfg_path)

    @staticmethod
    def _corr_evt(calc, site, det):
        obs_evt = calc.ibdCount(site, det)
        tot_bkg = calc.totalBkg(site, det)
        veto_eff = calc.vetoEff(site, det)
        mult_eff = calc.dmcEff(site, det)
        livetime = calc.livetime(site, det)

        obs_bkg = tot_bkg * livetime * veto_eff * mult_eff

        return (obs_evt - obs_bkg) / (livetime * veto_eff * mult_eff)

    @lru_cache(8)
    def ibd_eff(self, site, det):
        corr_rate = self._corr_evt(self.owner_calc, site, det)
        corr_rate_nom = self._corr_evt(self.nom_calc, site, det)

        return corr_rate / corr_rate_nom

    @staticmethod
    def _load_cut(config_path):
        config = ConfigFile(config_path)
        cut = VertexCut()
        cut.minZ = config.get("minZ", cut.minZ)
        cut.maxZ = config.get("maxZ", cut.maxZ)
        cut.minR = config.get("minR", cut.minR)
        cut.maxR = config.get("maxR", cut.maxR)
        return cut

    def _cyl_factor(self):
        nom_maxR = 1700         # 1500 + 200 reso
        maxR = min(self.cut.maxR, nom_maxR)
        included = (maxR / nom_maxR) ** 2
        excluded = (self.cut.minR / nom_maxR) ** 2
        return included - excluded

    def _z_factor_uniform(self):
        nom_minZ = -1700
        nom_maxZ = 1700
        minZ = max(self.cut.minZ, nom_minZ)
        maxZ = min(self.cut.maxZ, nom_maxZ)
        return (maxZ - minZ) / (nom_maxZ - nom_minZ)

    def _z_factor_amc(self):
        "Assume a linear profile from 0 to maxZ. See Fig 2 of 1512.00295."
        nom_minZ = 0
        # nom_maxZ = 1700
        nom_maxZ = 1500
        minZ = max(self.cut.minZ, nom_minZ)
        maxZ = min(self.cut.maxZ, nom_maxZ)
        # return (maxZ - minZ) / (nom_maxZ - nom_minZ)
        nom = maxZ**2 - minZ**2 - nom_minZ * (maxZ - minZ)
        denom = nom_maxZ**2 - nom_minZ * nom_maxZ
        return nom / denom

    def _uniform_eff(self):
        return self._z_factor_uniform() * self._cyl_factor()

    def _amc_eff(self):
        return self._z_factor_amc() * self._cyl_factor()

    def li9_eff(self):
        return self._uniform_eff()

    def fastn_eff(self):
        return self._uniform_eff()

    def amc_eff(self):
        return self._amc_eff()

    def alphan_eff(self):
        return self._uniform_eff()


class DummyVertexEffCalc:
    def __getattribute__(self, name):
        def unity(*a, **kw):
            return 1.
        return unity
