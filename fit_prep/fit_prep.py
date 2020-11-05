#!/usr/bin/env python3

# NOTE: This must be run from ibd_prod for now

import argparse
import os

from prod_util import data_dir

from hist_gen import gen_hists
from text_gen import gen_text


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('tag')
    ap.add_argument('config')
    ap.add_argument('--bcw', action='store_true',
                    help='Use BCW binning')
    ap.add_argument('--suffix')
    ap.add_argument('--pars-after-suffix', type=int)
    args = ap.parse_args()

    if args.suffix:
        if args.pars_after_suffix:
            parts = args.config.split("_")
            outconfig = "_".join(parts[:-args.pars_after_suffix]) + \
                f"@{args.suffix}_" + "_".join(parts[-args.pars_after_suffix:])
        else:
            outconfig = args.config + "@" + args.suffix
    else:
        outconfig = args.config

    outdir = data_dir('fit_input', f'{args.tag}@{outconfig}')
    os.system(f'mkdir -p {outdir}')

    gen_hists(args.tag, args.config, outconfig, bcw=args.bcw)
    gen_text(args.tag, args.config, outconfig)


if __name__ == '__main__':
    main()
