"Calculate the things"

from hardcoded import Hardcoded
# from delayed_eff import DelayedEffCalc
from prompt_eff import PromptEffCalc
from vertex_eff import VertexEffCalc, DummyVertexEffCalc

from prod_util import dets_for_phase, idet
from prod_util import stage2_pbp_path, configfile_path

from root_pandas import read_root
from numpy import sqrt
import ROOT as R


class Calc:
    "Calculate for a given phase"

    def __init__(self, phase, tag, config,
                 delayed_eff_mode="rel",
                 delayed_eff_impl="calc-then-add",
                 vtx_eff_nom_tagconf=None):
        self.phase = phase
        self.delayed_eff_mode = delayed_eff_mode
        self.delayed_eff_impl = delayed_eff_impl

        self.hardcoded = Hardcoded(phase)

        self.files, self.results = {}, {}
        for site in [1, 2, 3]:
            path = stage2_pbp_path(site, phase, tag, config)

            self.files[site] = R.TFile(path)

            results = read_root(path, 'results')
            for det in dets_for_phase(site, phase):
                self.results[(site, det)] = \
                    results.query(f'detector == {det}')

        cfg_path = configfile_path(tag, config)
        # self.delEffCalc = DelayedEffCalc(config_path, self)
        self.promptEffCalc = PromptEffCalc(cfg_path)

        if vtx_eff_nom_tagconf:
            nom_tag, nom_conf = vtx_eff_nom_tagconf.split("@")
            self.vertexEffCalc = VertexEffCalc(self, phase,
                                               tag, config,
                                               nom_tag, nom_conf)
        else:
            self.vertexEffCalc = DummyVertexEffCalc()

        if delayed_eff_impl == "add-then-calc":
            raise NotImplementedError("add-then-calc")

    def _livetime_weighted(self, site, det, var):
        r = self.results[(site, det)]
        return (r[var] * r.livetime_s).sum() / r.livetime_s.sum()

    def _livetime_weighted_squared(self, site, det, var):
        r = self.results[(site, det)]
        return sqrt((r[var]**2 * r.livetime_s).sum()) / r.livetime_s.sum()

    def _relDelEff(self, site, det):
        # return self.delEffCalc.scale_factor(self.phase, site, det)
        if self.delayed_eff_impl == "calc-then-add":
            return self._livetime_weighted(site, det, "delayedEffRel")
        else:
            raise NotImplementedError("add-then-calc")

    def _absDelEff(self, site, det):
        if self.delayed_eff_impl == "calc-then-add":
            return self._livetime_weighted(site, det, "delayedEffAbs")
        else:
            raise NotImplementedError("add-then-calc")

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

    def _li9Scale(self, site, det):
        scaleP = self.promptEffCalc.li9_rel_eff(site, det)
        scaleD = self._relDelEff(site, det)
        scaleV = self.vertexEffCalc.li9_eff()
        return scaleP * scaleD * scaleV

    def li9Bkg(self, site, det):
        scale = self._li9Scale(site, det)
        return scale * self._livetime_weighted(site, det, 'li9Daily')

    def li9BkgErr(self, site, det):
        scale = self._li9Scale(site, det)
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

    def _fastnScale(self, site, det):
        scaleP = self.promptEffCalc.fastn_rel_eff(site, det)
        scaleD = self._relDelEff(site, det)
        scaleV = self.vertexEffCalc.fastn_eff()
        return scaleP * scaleD * scaleV

    def fastnBkg(self, site, det):
        scale = self._fastnScale(site, det)
        return scale * self._hardcoded(site, det, 'fastnBkg')

    def fastnBkgErr(self, site, det):
        scale = self._fastnScale(site, det)
        return scale * self._hardcoded(site, det, 'fastnBkgErr')

    # TODO scale for modified delayed cut
    def _amcScale(self, site, det):
        scaleP = self.promptEffCalc.amc_rel_eff(site, det)
        scaleV = self.vertexEffCalc.amc_eff()
        return scaleP * scaleV

    def amcBkg(self, site, det):
        scale = self._amcScale(site, det)
        return scale * self._hardcoded(site, det, 'amcBkg')

    def amcBkgErr(self, site, det):
        scale = self._amcScale(site, det)
        return scale * self._hardcoded(site, det, 'amcBkgErr')

    def _alphanScale(self, site, det):
        scaleP = self.promptEffCalc.alphan_rel_eff(site, det)
        scaleD = self._relDelEff(site, det)
        scaleV = self.vertexEffCalc.alphan_eff()
        return scaleP * scaleD * scaleV

    def alphanBkg(self, site, det):
        scale = self._alphanScale(site, det)
        return scale * self._hardcoded(site, det, 'alphanBkg')

    def alphanBkgErr(self, site, det):
        scale = self._alphanScale(site, det)
        return scale * self._hardcoded(site, det, 'alphanBkgErr')

    def targetMass(self, site, det):
        return self._hardcoded(site, det, 'targetMass')

    def actualDelayedEff(self, site, det):
        if self.delayed_eff_mode == "rel":
            return self._relDelEff(site, det)
        elif self.delayed_eff_mode == "abs":
            return self._absDelEff(site, det)
        elif self.delayed_eff_mode == "flat":
            return 0.88
        else:
            raise Exception(f"delayed_eff_mode = {self.delayed_eff_mode}???")

    def vertexEff(self, site, det):
        return self.vertexEffCalc.ibd_eff(site, det)

    def delayedEff(self, site, det):
        # NOTE: Here we fold in the vertex efficiency for IBDs. Then
        # ToyMC/fitter will do the right thing.
        eff = self.actualDelayedEff(site, det)
        return eff * self.vertexEff(site, det)
        # return self._hardcoded(site, det, 'delayedEff')

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
