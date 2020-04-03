#!/usr/bin/env python3

# TODO: Make general, remove hardcoding of config space

import argparse
import os
import sys

from config_file import ConfigFile


def template_path():
    home = os.getenv("IBDSEL_HOME")
    return os.path.join(home, "static/configs/config.nominal.txt")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('-d', '--outdir',
                    default=os.getenv('IBDSEL_CONFIGDIR'))
    args = ap.parse_args()

    if args.outdir is None:
        print("Need either --outdir or $IBDSEL_CONFIGDIR")
        sys.exit(1)

    config = ConfigFile(template_path())

    for showerCut in [200_000, 400_000]:
        for showerTime in [300_000, 500_000]:
            config["showerMuChgCut"] = showerCut
            config["showerMuPostVeto_us"] = showerTime

            outname = f"config.muE_{showerCut}_muT_{showerTime}.txt"
            outpath = os.path.join(args.outdir, outname)

            config.write(outpath)


if __name__ == "__main__":
    main()
