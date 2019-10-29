#!/usr/bin/env python

"""Simple python macro to run stage1_main.cc"""

from load import ROOT as R
from stdvector import stdvector

def main(args):
    R.stage1_main(args.input, args.output, args.site)

    print('Done processing file', args.input)
    print('Write output file', args.output)

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('input', help='input files')
    parser.add_argument('-s', '--site', type=int, choices=(1,2,3), required=True, help='site to process')
    parser.add_argument('-o', '--output', required=True, help='output file name')

    main(parser.parse_args())
