#!/usr/bin/env python3

import argparse

import ROOT as R

from prod_util import dets_for_phase, stage2_pbp_path
from prod_util import idet as get_idet

from util import read_theta13_file


def get_ncap_spec(phase, site, det, calc, vtxcut,
                  tag="2021_02_03", config="del4MeV"):
    stage2_file = stage2_pbp_path(site, phase, tag, config)

    nADs = [6, 8, 7][phase-1]

    hname = f"h_ncap_{nADs}ad_eh{site}_ad{det}"
    htitle = f"Neutron capture spectrum, EH{site}-AD{det} ({nADs}AD)"
    h = R.TH1F(hname, htitle, 160, 4, 12)

    tree = stage2_file.Get(f"ibd_AD{det}")
    sel = f"zP >= {vtxcut.minZ} && zD >= {vtxcut.minZ}"
    sel += f" && zP <= {vtxcut.maxZ} && zD <= {vtxcut.maxZ}"
    sel += f" && sqrt(xP*xP + yP*yP) >= {vtxcut.minR}"
    sel += f" && sqrt(xD*xD + yD*yD) >= {vtxcut.minR}"
    sel += f" && sqrt(xP*xP + yP*yP) <= {vtxcut.maxR}"
    sel += f" && sqrt(xD*xD + yD*yD) <= {vtxcut.maxR}"
    tree.Draw(f"eD>>{hname}", sel, "goff")

    h_sing_orig = stage2_file.Get(f"h_single_AD{det}")
    h_sing = h.Clone("h_sing")
    h_sing.Reset()

    for xbin in range(h_sing_orig.FindBin(4), h_sing_orig.FindBin(12)+1):
        h_sing.Fill(h_sing_orig.GetBinCenter(xbin),
                    h_sing_orig.GetBinContent(xbin))

    h_sing.Scale(1/h_sing.Integral())

    veto_eff = calc.vetoEff(site, det)
    mult_eff = calc.multEff(site, det)
    livetime = calc.livetime(site, det)
    acc_bkg = calc.accBkg(site, det)

    total_acc = acc_bkg * livetime * veto_eff * mult_eff
    h.Add(h_sing, -total_acc)
    del h_sing

    return h
