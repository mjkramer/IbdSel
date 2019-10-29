#!/usr/bin/env python

"""Simple python macro to imitate ibd_main.cc workflow"""

from load import ROOT as R
from stdvector import stdvector

def main(args):
    p = R.Pipeline()

    p.makeOutFile(args.output)

    p.makeAlg('EventReader')()
    p.makeAlg('TrigTypeCut')()
    p.makeAlg('MuonAlg')()
    p.makeAlg('FlasherCut')()

    detectors = list(range(1, 5))
    for det in detectors:
        R.makeAlg('ClusterAlg')(p, det)
        R.makeAlg('SelectIBD')(p, det)
        R.makeAlg('SelectSingles')(p, det)

    p.makeTool('MultCutTool')()

    v = stdvector(args.input)
    p.process(v)

    print('Done processing file', args.input)
    print('Write output file', args.output)

if __name__ == "__main__":
    from argparse import ArgumentParser
    parser = ArgumentParser()
    parser.add_argument('input', nargs='+', default=(), help='input files')
    parser.add_argument('-o', '--output', required=True, help='output file name')

    main(parser.parse_args())
