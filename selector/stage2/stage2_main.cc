#include "stage2_main.hh"

#include "AdBuffer.hh"
#include "CalcsTree.hh"
#include "Calculator.hh"
#include "Li9Calc.hh"
#include "MultCut.hh"
#include "MuonAlg.hh"
#include "Readers.hh"
#include "Selectors.hh"

#include "../common/Misc.hh"

using CM = ClockMode;

void stage2_main(const char* confFile, const char* inFile, const char* outFile,
                 Site site, Phase phase, UInt_t seq)
{
  Pipeline p;

  p.makeOutFile(outFile);

  p.makeTool<Config>(confFile);

  p.makeAlg<MuonReader>();
  p.makeAlg<MuonAlg>(MuonAlg::Purpose::ForIBDs);
  p.makeAlg<MuonAlg>(MuonAlg::Purpose::ForSingles);
  p.makeAlg<PrefetchLooper<MuonReader>>(); // build up a lookahead buffer

  std::vector<Det> allADs = util::ADsFor(site, phase);

  for (const Det detector : allADs) {
    const bool lastAD = detector == allADs.back();

    p.makeAlg<AdReader>(detector, lastAD ? CM::ClockWriter : CM::ClockReader);
    p.makeAlg<AdBuffer>(detector);

    p.makeAlg<SingleSel>(detector);
    p.makeAlg<IbdSel>(detector);

    if (not lastAD)
      p.makeAlg<PrefetchLooper<AdReader, Det>>(detector);
  }

  // Clock is needed for TimeSyncTool (i.e. all the readers)
  p.makeTool<Clock>();

  p.makeTool<MultCutTool>();

  p.process({inFile});

  Calculator calc(p, phase, seq, site);
  calc.writeValues();
}
