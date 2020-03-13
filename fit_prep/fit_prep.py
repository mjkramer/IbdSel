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
    args = ap.parse_args()

    outdir = data_dir('fit_input', f'{args.tag}@{args.config}')
    os.system(f'mkdir -p {outdir}')

    gen_hists(args.tag, args.config)
    gen_text(args.tag, args.config)


if __name__ == '__main__':
    main()
