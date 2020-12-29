"Utilities"

import ROOT as R

import os

__T13_ROWS = {
    1: "obs_evt",
    2: "livetime",
    3: "veto_eff",
    4: "mult_eff",
    5: "delcut_eff",
    6: "power_unc",
    7: "tot_eff_unc",
    8: "target_mass",
    9: "tot_bkg",
    10: "tot_bkg_unc",
    11: "acc_bkg",
    12: "acc_bkg_unc",
    13: "li9_bkg",
    14: "li9_bkg_unc",
    15: "fastn_bkg",
    16: "fastn_bkg_unc",
    17: "amc_bkg",
    18: "amc_bkg_unc",
    19: "alphan_bkg",
    20: "alphan_bkg_unc"
}


def read_theta13_file(fname):
    "Returns {rowname: [valEH1AD1, ..., valEH3AD4]}"
    result = {}
    headers_remaining = 3

    for line in open(fname):
        line = line.strip()

        if line.startswith("#") or not line:
            continue

        if headers_remaining > 0:
            headers_remaining -= 1
            continue

        words = line.split()
        rownum = int(words[1])
        if rownum == 0:         # timestamps etc
            continue
        vals = map(float, words[2:])
        result[__T13_ROWS[rownum]] = list(vals)

    return result


def load_selector():
    direc = os.path.join(os.getenv('IBDSEL_HOME'),
                         'selector/_build')

    for lib in ['common', 'stage1', 'stage2']:
        R.gSystem.Load(f'{direc}/{lib}.so')
