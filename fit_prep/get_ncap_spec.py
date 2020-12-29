#!/usr/bin/env python3

import argparse

import ROOT as R

from prod_util import dets_for_phase
from prod_util import idet as get_idet

from util import read_theta13_file


def get_ncap_spec(stage2_file, t13_file, nADs, site, det):
    hname = f"h_ncap_{nADs}ad_eh{site}_ad{det}"
    htitle = f"Neutron capture spectrum, EH{site}-AD{det} ({nADs}AD)"
    h = R.TH1F(hname, htitle, 160, 4, 12)

    tree = stage2_file.Get(f"ibd_AD{det}")
    tree.Draw(f"eD>>{hname}", "", "goff")

    h_sing_orig = stage2_file.Get(f"h_single_AD{det}")
    h_sing = h.Clone("h_sing")
    h_sing.Reset()

    for xbin in range(h_sing_orig.FindBin(4), h_sing_orig.FindBin(12)+1):
        h_sing.Fill(h_sing_orig.GetBinCenter(xbin),
                    h_sing_orig.GetBinContent(xbin))

    h_sing.Scale(1/h_sing.Integral())

    idet = get_idet(site, det)

    veto_eff = t13_file["veto_eff"][idet]
    mult_eff = t13_file["mult_eff"][idet]
    livetime = t13_file["livetime"][idet]
    acc_bkg = t13_file["acc_bkg"][idet]  # per day, assuming perfect eff

    total_acc = acc_bkg * livetime * veto_eff * mult_eff
    h.Add(h_sing, -total_acc)
    del h_sing

    return h


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("stage2_pbp_dir")
    ap.add_argument("fit_input_dir")
    ap.add_argument("outfile")
    args = ap.parse_args()

    outfile = R.TFile(args.outfile, "RECREATE")

    for phase in [1, 2, 3]:
        nADs = [6, 8, 7][phase-1]
        fname_txt = f"Theta13-inputs_P17B_inclusive_{nADs}ad.txt"
        t13_file = read_theta13_file(f"{args.fit_input_dir}/{fname_txt}")

        for site in [1, 2, 3]:
            fname = f"stage2.pbp.eh{site}.{nADs}ad.root"
            infile = R.TFile(f"{args.stage2_pbp_dir}/{fname}")

            for det in dets_for_phase(site, phase):
                h_ncap = get_ncap_spec(infile, t13_file, nADs, site, det)
                outfile.cd()
                h_ncap.Write()


if __name__ == '__main__':
    main()
