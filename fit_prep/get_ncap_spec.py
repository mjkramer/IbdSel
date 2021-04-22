#!/usr/bin/env python3

# NOTE: Retired, see comments in delayed_eff

import argparse

import ROOT as R

from prod_util import dets_for_phase, stage2_pbp_path
from prod_util import idet as get_idet

from util import read_theta13_file


def keep(o):
    R.SetOwnership(o, False)     # don't delete it, python!
    try:
        o.SetDirectory(R.gROOT)  # don't delete it, root!
        # o.SetDirectory(0)
    except Exception:
        pass                     # unless you weren't going to anyway
    return o


def get_ncap_spec(calc, site, det):
    stage2_path = stage2_pbp_path(site, calc.phase, calc.tag, calc.config)
    stage2_file = R.TFile(stage2_path)

    nADs = [6, 8, 7][calc.phase-1]

    hname = f"h_ncap_{nADs}ad_eh{site}_ad{det}"
    htitle = f"Neutron capture spectrum, EH{site}-AD{det} ({nADs}AD)"
    h = R.TH1F(hname, htitle, 160, 4, 12)

    tree = stage2_file.Get(f"ibd_AD{det}")
    # sel = f"zP >= {vtxcut.minZ} && zD >= {vtxcut.minZ}"
    # sel += f" && zP <= {vtxcut.maxZ} && zD <= {vtxcut.maxZ}"
    # sel += f" && sqrt(xP*xP + yP*yP) >= {vtxcut.minR}"
    # sel += f" && sqrt(xD*xD + yD*yD) >= {vtxcut.minR}"
    # sel += f" && sqrt(xP*xP + yP*yP) <= {vtxcut.maxR}"
    # sel += f" && sqrt(xD*xD + yD*yD) <= {vtxcut.maxR}"
    sel = ""
    tree.Draw(f"eD>>{hname}", sel, "goff")

    h_sing_orig = stage2_file.Get(f"h_single_AD{det}")
    h_sing = h.Clone("h_sing")
    h_sing.Reset()

    for xbin in range(h_sing_orig.FindBin(4), h_sing_orig.FindBin(12)+1):
        h_sing.Fill(h_sing_orig.GetBinCenter(xbin),
                    h_sing_orig.GetBinContent(xbin))

    h_sing.Scale(1/h_sing.Integral())

    veto_eff = calc.vetoEff(site, det)
    mult_eff = calc.dmcEff(site, det)
    livetime = calc.livetime(site, det)
    acc_bkg = calc.accBkg(site, det)

    total_acc = acc_bkg * livetime * veto_eff * mult_eff
    h.Add(h_sing, -total_acc)
    del h_sing

    return keep(h)
