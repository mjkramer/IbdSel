#!/usr/bin/env python3

# NOTE: This must be run from ibd_prod for now

import argparse
import os

from prod_util import data_dir

from hist_gen import gen_hists
from text_gen import gen_text
from util import load_selector


load_selector()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('tag')
    ap.add_argument('config')
    ap.add_argument('--bcw', action='store_true',
                    help='Use BCW binning (deprecated, use --binning instead)')
    ap.add_argument('--binning', choices=['lbnl', 'bcw', 'ihep'], default='lbnl')
    ap.add_argument('--suffix')
    ap.add_argument('--pars-after-suffix', type=int)
    ap.add_argument('--delayed-eff-mode', choices=["abs", "rel", "flat"],
                    default="rel")
    ap.add_argument('--delayed-eff-impl',
                    # new is synonymous with add-then calc
                    choices=["add-then-calc", "calc-then-add", "old", "new", "none"],
                    default="add-then-calc")
    ap.add_argument('--delayed-eff-ref',
                    help="tag@config to use as reference for delayed eff (old)")
    ap.add_argument('--vtx-eff-ref',
                    help="tag@config to use as reference for vertex eff")
    ap.add_argument('--outconfig',
                    help='Manually specify name of output config')
    args = ap.parse_args()

    # Keep --bcw around for backward compatibility
    if args.bcw:
        args.binning = 'bcw'

    if args.suffix:
        if args.pars_after_suffix:
            parts = args.config.split("_")
            outconfig = "_".join(parts[:-args.pars_after_suffix]) + \
                f"@{args.suffix}_" + "_".join(parts[-args.pars_after_suffix:])
        else:
            outconfig = args.config + "@" + args.suffix
    else:
        outconfig = args.config

    if args.outconfig:
        outconfig = args.outconfig

    outdir = data_dir('fit_input', f'{args.tag}@{outconfig}')
    os.system(f'mkdir -p {outdir}')

    gen_hists(args.tag, args.config, outconfig, args.binning)
    gen_text(args.tag, args.config, outconfig,
             args.delayed_eff_mode, args.delayed_eff_impl,
             args.delayed_eff_ref,
             args.vtx_eff_ref)


if __name__ == '__main__':
    main()
