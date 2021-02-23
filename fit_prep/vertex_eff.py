from dataclasses import dataclass
from functools import lru_cache

from root_pandas import read_root

from config_file import ConfigFile
from prod_util import dets_for_phase, stage2_pbp_path


@dataclass
class VertexCut:
    minZ: float = -10_000
    maxZ: float = 10_000
    minR: float = 0
    maxR: float = 10_000


class VertexEffCalc:
    def __init__(self, config_path, phase,
                 reftag="2021_02_03", refconfig="yolo5"):
        self.dfs = self._load_dfs(phase, reftag, refconfig)
        self.cut = self._load_cut(config_path)

    @staticmethod
    def _load_dfs(phase, reftag, refconfig):
        result = {}
        for site in [1, 2, 3]:
            path = stage2_pbp_path(site, phase, reftag, refconfig)
            for det in dets_for_phase(site, phase):
                result[(site, det)] = read_root(path, f"ibd_AD{det}")
        return result

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
        "XXX FIXME Don't assume uniform distribution in top half. Use \
        something based on the AmC paper."
        nom_minZ = 0
        nom_maxZ = 1700
        minZ = max(self.cut.minZ, nom_minZ)
        maxZ = min(self.cut.maxZ, nom_maxZ)
        return (maxZ - minZ) / (nom_maxZ - nom_minZ)

    def _uniform_eff(self):
        return self._z_factor_uniform() * self._cyl_factor()

    def _amc_eff(self):
        return self._z_factor_amc() * self._cyl_factor()

    @lru_cache(8)
    def ibd_eff(self, site, det):
        df = self.dfs[(site, det)]
        cut = self.cut
        cond_z = f"{cut.minZ} <= zP <= {cut.maxZ}"
        cond_z += f" and {cut.minZ} <= zD <= {cut.maxZ}"
        cond_r = f"{cut.minR} <= sqrt(xP**2 + yP**2) <= {cut.maxR}"
        cond_r += f" and {cut.minR} <= sqrt(xD**2 + yD**2) <= {cut.maxR}"

        num = len(df.query(f"{cond_z} and {cond_r}"))
        return num / len(df)

    def li9_eff(self):
        return self._uniform_eff()

    def fastn_eff(self):
        return self._uniform_eff()

    def amc_eff(self):
        return self._amc_eff()

    def alphan_eff(self):
        return self._uniform_eff()
