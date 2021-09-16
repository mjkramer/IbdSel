import numpy as np
import ROOT as R

from config_file import ConfigFile
from prod_util import configfile_path, dets_for_phase, stage2_pbp_path
from prod_util import fit_hist_ibd_path, fit_hist_acc_path

nbins_lbnl = 37
nbins_bcw = 26
nbins_fine = 240


def min_energy(tag, config):
    cfg = ConfigFile(configfile_path(tag, config))
    return cfg["ibdPromptEmin"]


def binning_lbnl(emin=0.7):
    edges = np.concatenate([[emin],
                            np.arange(1, 8, 0.2),
                            [8, 12]])
    assert len(edges) == 1 + nbins_lbnl
    return edges


def binning_bcw(emin=0.7):
    edges = np.concatenate([[emin],
                            np.arange(1.3, 7.3, 0.25),
                            [7.3, 12]])
    assert len(edges) == 1 + nbins_bcw
    return edges


def binning_fine():
    return np.linspace(0, 12, 1 + nbins_fine)


def gen_hists_(phase, tag, config, outconfig, bcw=False):
    path_ibd = fit_hist_ibd_path(phase, tag, outconfig)
    path_acc = fit_hist_acc_path(phase, tag, outconfig)
    f_ibd = R.TFile(path_ibd, 'RECREATE')
    f_acc = R.TFile(path_acc, 'RECREATE')

    for site in [1, 2, 3]:
        path_in = stage2_pbp_path(site, phase, tag, config)
        f = R.TFile(path_in)

        # fitter expects all 8 dets in the file, even in 6/7AD periods
        for det in dets_for_phase(site, 2):  # phase 2 = 8AD
            h_ibd = f.Get(f'h_ibd_AD{det}')
            h_acc = f.Get(f'h_single_AD{det}')

            suffix = f'eh{site}_ad{det}'
            name_ibd_coarse = f'h_ibd_eprompt_inclusive_{suffix}'
            name_ibd_fine = f'h_ibd_eprompt_fine_inclusive_{suffix}'
            name_acc_coarse = f'h_accidental_eprompt_inclusive_{suffix}'
            name_acc_fine = f'h_accidental_eprompt_fine_inclusive_{suffix}'

            nbins = nbins_bcw if bcw else nbins_lbnl
            emin = min_energy(tag, config)
            binning = binning_bcw(emin) if bcw else binning_lbnl(emin)

            if h_ibd:
                h_ibd_coarse = h_ibd.Rebin(nbins, name_ibd_coarse,
                                           binning)
                h_ibd_fine = h_ibd.Rebin(nbins_fine, name_ibd_fine,
                                         binning_fine())
                h_acc_coarse = h_acc.Rebin(nbins, name_acc_coarse,
                                           binning)
                h_acc_fine = h_acc.Rebin(nbins_fine, name_acc_fine,
                                         binning_fine())
            else:
                h_ibd_coarse = R.TH1F(name_ibd_coarse, name_ibd_coarse,
                                      nbins, binning)
                h_ibd_fine = R.TH1F(name_ibd_fine, name_ibd_fine,
                                    nbins_fine, binning_fine())
                h_acc_coarse = R.TH1F(name_acc_coarse, name_acc_coarse,
                                      nbins, binning)
                h_acc_fine = R.TH1F(name_acc_fine, name_acc_fine,
                                    nbins_fine, binning_fine())

            f_ibd.cd()
            h_ibd_coarse.Write()
            h_ibd_fine.Write()

            f_acc.cd()
            h_acc_coarse.Write()
            h_acc_fine.Write()

    f_ibd.Close()
    f_acc.Close()


def gen_hists(tag, config, outconfig, bcw=False):
    for phase in [1, 2, 3]:
        try:
            gen_hists_(phase, tag, config, outconfig, bcw=bcw)
        except Exception:
            print(f"gen_hists: Skipping phase {phase}")
