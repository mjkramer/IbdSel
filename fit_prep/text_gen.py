"Generate the 'theta13' file"

import traceback

from calc import Calc, DummyCalc

from prod_util import sitedets, dets_for_phase
from prod_util import fit_text_input_path

from datetime import datetime as DT
import os


def today():
    return DT.now().strftime('%b. %-m, %Y')


def gen_text_(out_fname, phase, tag, config,
              delayed_eff_mode, delayed_eff_impl,
              delayed_eff_ref=None,
              vtx_eff_nom_tagconf=None):
    try:
        calc = Calc(phase, tag, config,
                    delayed_eff_mode, delayed_eff_impl,
                    delayed_eff_ref,
                    vtx_eff_nom_tagconf)
    except Exception:
        calc = DummyCalc(phase)

    outf = open(out_fname, 'w')

    def w(s):
        outf.write(s + '\n')

    def _dump(irow, vals, fmt='%s'):
        valstr = '\t'.join(fmt % v for v in vals)
        w(f'{phase}\t{irow}\t{valstr}')

    def dump(irow, func, fmt):
        nonzero_rows = [8]      # target mass
        vals = []
        for site, det in sitedets():
            if det in dets_for_phase(site, phase) or irow in nonzero_rows:
                vals.append(func(site, det))
            else:
                vals.append(0)

        _dump(irow, vals, fmt)

    w(f'# Produced on {today()} by IbdSel')
    w('# For P17B analysis')
    w('#')
    w('# Stage')
    w(f'{phase}')
    w('# DataFlag 1=Data, 0=MC')
    w('1')
    w('# Delta M^2_{32} and error (eV^2) (not used in our analysis)')
    w('0.00243  0.00013')
    w('#')
    w('# ===============================================================')
    w('# First column is time period number, second column is row number')
    w('#')
    w('# Columns are organzed as:')
    w('# 1-1,   1-2,   2-1,   2-2,   3-1,   3-2,   3-3,   3-4')
    w('# (AD1), (AD2), (AD3), (AD8), (AD4), (AD5), (AD6), (AD7)')
    w('# ===============================================================')
    w('#')
    w('# Row 0 ==>  Start UTC  |  End UTC | Start date and time')
    _dump(0, calc.timestamps())
    w('#')
    w('# Row 1 ==>  Observed number of events in AD1 to AD8 ')
    dump(1, calc.ibdCount, '%d')
    w('#')
    w('# Row 2 ==>  Live time in days for AD1 to AD8')
    dump(2, calc.livetime, '%.3f')
    w('#')
    w('# Row 3 ==>  Muon veto efficiency for AD1 to AD8')
    dump(3, calc.vetoEff, '%.4f')
    w('#')
    w('# Row 4 ==>  Multiplicity cut efficiency for AD1 to AD8')
    dump(4, calc.dmcEff, '%.6f')
    w('#')
    w('# Row 5 ==>  6 MeV neutron cut efficiency for AD1 to AD8')
    dump(5, calc.delayedEff, '%.4f')
    w('#')
    w('# Row 6 ==>  Uncorrelated reactor power uncertainty (relative error in %)')
    dump(6, calc.powerErr, '%.1f')
    w('#')
    w('# Row 7 ==>  Total IBD cut efficiency uncertainty (relative error in %)')
    dump(7, calc.totEffErr, '%.1f')
    w('#')
    w('# Row 8 ==>  Total target mass for AD1 to AD8 (kg)')
    dump(8, calc.targetMass, '%.6g')
    w('#')
    w('#### Note: The background estimates are *not* corrected for efficiencies anymore')
    w('#')
    w('# Row 9  ==>  Expected number of bkg events per live day (AD1 to AD8)')
    w('# Row 10 ==>  Absolute uncertainty on the bkg estimates per live day')
    dump(9, calc.totalBkg, '%.5g')
    dump(10, calc.totalBkgErr, '%.5g')
    w('#')
    w('# Row 11 ==>  Expected number of accidental events per live day (AD1 to AD8)')
    w('# Row 12 ==>  Absolute uncertainty on the accidental bkg per live day')
    dump(11, calc.accBkg, '%.4g')
    dump(12, calc.accBkgErr, '%.4g')
    w('#')
    w('# Row 13 ==>  Expected number of li9/he8 events per live day (AD1 to AD8)')
    w('# Row 14 ==>  Absolute uncertainty on the li9/he8 bkg per live day')
    dump(13, calc.li9Bkg, '%.3g')
    dump(14, calc.li9BkgErr, '%.3g')
    w('#')
    w('# Row 15 ==>  Expected number of fast-n events per live day (AD1 to AD8)')
    w('# Row 16 ==>  Absolute uncertainty on the fast-n bkg per live day')
    dump(15, calc.fastnBkg, '%.4g')
    dump(16, calc.fastnBkgErr, '%.4g')
    w('#')
    w('# Row 17 ==>  Expected number of AmC-corr events per live day (AD1 to AD8)')
    w('# Row 18 ==>  Absolute uncertainty on the AmC-corr bkg per live day')
    dump(17, calc.amcBkg, '%.3g')
    dump(18, calc.amcBkgErr, '%.3g')
    w('#')
    w('# Row 19 ==>  Expected number of alpha-n events per live day (AD1 to AD8)')
    w('# Row 20 ==>  Absolute uncertainty on the alpha-n bkg per live day')
    dump(19, calc.alphanBkg, '%.3g')
    dump(20, calc.alphanBkgErr, '%.3g')

    outf.close()

    auxname = "aux_" + os.path.basename(out_fname)
    auxpath = os.path.dirname(out_fname) + "/" + auxname
    outf_aux = open(auxpath, "w")

    def dump_aux(name, func, fmt):
        vals = []
        for site, det in sitedets():
            if det in dets_for_phase(site, phase):
                vals.append(func(site, det))
            else:
                vals.append(0)

        line = f"{name}\t" + "\t".join([fmt % v for v in vals])
        outf_aux.write(line + "\n")

    dump_aux("delayedEff", calc.actualDelayedEff, "%.4f")
    dump_aux("vertexEff", calc.vertexEff, "%.4f")


def gen_text(tag, config, outconfig, *a, **kw):
    for phase in [1, 2, 3]:
        try:
            out_fname = fit_text_input_path(phase, tag, outconfig)
            gen_text_(out_fname, phase, tag, config, *a, **kw)
        except Exception:
            print(f"Warning: gen_text: Skipping phase {phase} because:")
            print(traceback.format_exc())
