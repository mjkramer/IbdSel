import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gSystem.Load('build/libIbdSel.so')

# ROOT.TH1.AddDirectory( False )
# ROOT.gDirectory.AddDirectory( False )
