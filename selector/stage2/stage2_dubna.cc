#include "stage2_dubna.hh"

#include "AdBuffer.hh"
#include "CalcsTree.hh"
#include "Calculator.hh"
#include "Li9Calc.hh"
#include "MultCut.hh"
#include "MuonAlg.hh"
#include "Readers.hh"
#include "Selectors.hh"

#include "Misc.hh"

using CM = ClockMode;

// namespace stage2 { Site site; Phase phase; }

void stage2_dubna(const char* confFile, const char* inFile, const char* outFile,
                 Site site, Phase phase, UInt_t seq)
{
  // stage2::site = site;
  // stage2::phase = phase;

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

  p.makeTool<MultCutToolDubna>();

  p.process({inFile});

  Calculator calc(p, site, phase, seq);
  calc.writeEntries();
}
