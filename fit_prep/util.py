"Utilities"

import ROOT as R

import os


def load_selector():
    direc = os.path.join(os.getenv('IBDSEL_HOME'),
                         'selector/_build')

    for lib in ['common', 'stage1', 'stage2']:
        R.gSystem.Load(f'{direc}/{lib}.so')
