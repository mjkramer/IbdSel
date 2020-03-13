import numpy as np
import ROOT as R

from prod_util import dets_for_phase, stage2_pbp_path
from prod_util import fit_hist_ibd_path, fit_hist_acc_path

nbins_lbnl = 37
nbins_fine = 240


def binning_lbnl():
    edges = np.concatenate([[0.7],
                            np.arange(1, 8, 0.2),
                            [8, 12]])
    assert len(edges) == 1 + nbins_lbnl
    return edges


# We don't actually need this since stage2 gives us this binning already
def binning_fine():
    return np.linspace(0, 12, 1 + nbins_fine)


def gen_hists_(phase, tag, config):
    path_ibd = fit_hist_ibd_path(phase, tag, config)
    path_acc = fit_hist_acc_path(phase, tag, config)
    f_ibd = R.TFile(path_ibd, 'RECREATE')
    f_acc = R.TFile(path_acc, 'RECREATE')

    for site in [1, 2, 3]:
        path_in = stage2_pbp_path(site, phase, tag, config)
        f = R.TFile(path_in)

        for det in dets_for_phase(site, phase):
            h_ibd = f.Get(f'h_ibd_AD{det}')
            h_acc = f.Get(f'h_single_AD{det}')

            name_ibd_lbnl = f'h_ibd_eprompt_inclusive_eh{site}_ad{det}'
            name_ibd_fine = f'h_ibd_eprompt_fine_inclusive_eh{site}_ad{det}'
            name_acc_lbnl = f'h_accidental_eprompt_inclusive_eh{site}_ad{det}'
            name_acc_fine = f'h_accidental_eprompt_fine_inclusive_eh{site}_ad{det}'

            h_ibd_lbnl = h_ibd.Rebin(nbins_lbnl, name_ibd_lbnl,
                                     binning_lbnl())
            h_ibd_fine = h_ibd.Rebin(nbins_fine, name_ibd_fine,
                                     binning_fine())
            h_acc_lbnl = h_acc.Rebin(nbins_lbnl, name_acc_lbnl,
                                     binning_lbnl())
            h_acc_fine = h_acc.Rebin(nbins_fine, name_acc_fine,
                                     binning_fine())

            f_ibd.cd()
            h_ibd_lbnl.Write()
            h_ibd_fine.Write()

            f_acc.cd()
            h_acc_lbnl.Write()
            h_acc_fine.Write()

    f_ibd.Close()
    f_acc.Close()


def gen_hists(tag, config):
    for phase in [1, 2, 3]:
        gen_hists_(phase, tag, config)
