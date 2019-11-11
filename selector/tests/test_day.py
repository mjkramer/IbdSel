import ROOT as R

for line in ['.x LoadBoost.C',
             '.L tests/FileFinder.cc+',
             ".L stage1_main.cc+"]:
    R.gROOT.ProcessLine(line)

R.Stage, R.Site                 # preload

FF = R.FileFinder()

def process(fileno):
    outpath = "tests/out.test_day/stage1.%04d.root" % fileno
    inpath = FF.find(21221, fileno)
    R.stage1_main(inpath, outpath, R.k6AD, R.EH1)

def go():
    for fileno in range(58, 210 + 1):
        print(fileno)
        process(fileno)
