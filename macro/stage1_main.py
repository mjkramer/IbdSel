#!/usr/bin/env python

"""Simple python macro to imitate stage1_main.cc workflow"""

from load import ROOT as R
from stdvector import stdvector

def main(args):
    p = R.Pipeline()

    p.makeOutFile(args.output)

    p.makeAlg('EventReader')()
    p.makeAlg('TrigTypeCut')()
    p.makeAlg('MuonSaver')()
    p.makeAlg('FlasherCut')()

    detectors = list(range(1, 5 if args.site==3 else 2))
    for det in detectors:
        R.makeAlg('ClusterSaver')(p, det)

    p.makeAlg('LivetimeSaver')()

    v = stdvector(args.input)
    p.process(v)

    print('Done processing file', args.input)
    print('Write output file', args.output)

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('input', nargs='+', default=(), help='input files')
    parser.add_argument('-s', '--site', type=int, choices=(1,2,3), required=True, help='site to process')
    parser.add_argument('-o', '--output', required=True, help='output file name')

    main(parser.parse_args())
