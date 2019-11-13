#include "Readers.cc"
#include "MuonAlg.cc"
#include "Selectors.cc"
#include "MultCut.cc"
#include "Calculator.cc"

#include "SelectorFramework/core/Kernel.cc"
#include "SelectorFramework/core/ConfigTool.cc"

void stage2_main(const char* confFile, const char* inFile, const char* outFile,
                 Stage stage, UInt_t seq, Site site)
{
  Pipeline p;

  p.makeOutFile(outFile);

  p.makeTool<Config>(confFile);

  p.makeAlg<MuonReader>();
  p.makeAlg<MuonAlg>(MuonAlg::Purpose::ForIBDs);
  p.makeAlg<MuonAlg>(MuonAlg::Purpose::ForSingles);
  p.makeAlg<PrefetchLooper<MuonReader>>(); // build up a lookahead buffer

  std::vector<Det> allADs = util::ADsFor(site, stage);

  for (Det detector : allADs) {
    bool firstAD = detector == allADs[0];

    p.makeAlg<SingleReader>(detector, firstAD); // ClockWriter if firstAD
    p.makeAlg<SingleSelector>(detector);

    p.makeAlg<ClusterReader>(detector);
    p.makeAlg<IbdSelector>(detector);
  }

  // Clock is needed for TimeSyncTool (i.e. all the readers)
  p.makeTool<Clock>();

  p.makeTool<MultCutTool>();

  p.process({inFile});

  Calculator calc(p, stage, seq, site);
  calc.writeValues();
}
