#!/usr/bin/env python

"""Simple python macro to run ibd_main.cc"""

from load import ROOT as R
from stdvector import stdvector

def main(args):
    R.ibd_main(stdvector(args.input), args.output)

    print('Done processing file', args.input)
    print('Write output file', args.output)

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('input', nargs='+', default=(), help='input files')
    parser.add_argument('-o', '--output', required=True, help='output file name')

    main(parser.parse_args())
