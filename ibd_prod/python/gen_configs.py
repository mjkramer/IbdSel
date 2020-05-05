#!/usr/bin/env python3

# TODO: Make general, remove hardcoding of config space

import argparse
import os
import sys

from config_file import ConfigFile


def template_path():
    home = os.getenv("IBDSEL_HOME")
    return os.path.join(home, "static/configs/config.nominal.txt")


def shower(config, outdir, ident=None):
    if ident is None:
        ident = "shower"

    for showerCut in [200_000, 300_000, 400_000]:
        for showerTime in [300_000, 400_000, 500_000]:
            config["ibdShowerMuChgCut"] = showerCut
            config["ibdShowerMuPostVeto_us"] = showerTime

            outname = f"config.{ident}_muE_{showerCut}_muT_{showerTime}.txt"
            config.write(os.path.join(outdir, outname))


def accIsol(config, outdir, ident=None):
    if ident is None:
        ident = "accIsol"

    for usec_before in [400, 700, 1000]:
        for usec_after in [200, 400, 700, 1000]:
            for emin_after in [0.7, 3, 8, 10]:
                config["singleDmcUsecBefore"] = usec_before
                config["singleDmcUsecAfter"] = usec_after
                config["singleDmcEminAfter"] = emin_after

                outname = f"config.{ident}_{usec_before}_{usec_after}_{emin_after}.txt"
                config.write(os.path.join(outdir, outname))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('studyname', choices=['shower', 'accIsol'])
    ap.add_argument('-d', '--outdir',
                    default=os.getenv('IBDSEL_CONFIGDIR'))
    ap.add_argument('-i', '--identifier')
    args = ap.parse_args()

    if args.outdir is None:
        print("Need either --outdir or $IBDSEL_CONFIGDIR")
        sys.exit(1)

    config = ConfigFile(template_path())
    os.system(f"mkdir -p {args.outdir}")
    eval(args.studyname)(config, args.outdir, args.identifier)


if __name__ == "__main__":
    main()
