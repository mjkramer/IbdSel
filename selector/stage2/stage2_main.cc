#include "stage2_main.hh"

#include "AdBuffer.hh"
#include "CalcsTree.hh"
#include "Calculator.hh"
#include "Li9Calc.hh"
#include "MultCut.hh"
#include "MuonAlg.hh"
#include "Readers.hh"
#include "Selectors.hh"
#include "VertexCut.hh"

#include "../common/Misc.hh"

using CM = ClockMode;

namespace stage2 { Site site; Phase phase; }

void stage2_main(const char* confFile, const char* inFile, const char* outFile,
                 Site site, Phase phase, UInt_t seq)
{
  stage2::site = site;
  stage2::phase = phase;

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

    // For evaluating delayed cut efficiency:
    // NB: Even though we're not changing the energy range for this singles
    // selection, we ARE using a different DMC
    p.makeAlg<SingleSel>(detector, TAG_LOW_DELAYED_EMIN);
    IbdSel& sel = p.makeAlg<IbdSel>(detector, TAG_LOW_DELAYED_EMIN);
    sel.DELAYED_MIN = LOW_DELAYED_EMIN;

    if (not lastAD)
      p.makeAlg<PrefetchLooper<AdReader, Det>>(detector);
  }

  // Clock is needed for TimeSyncTool (i.e. all the readers)
  p.makeTool<Clock>();

  p.makeTool<MultCutTool>();
  p.makeTool<VertexCutTool>();

  // For evaluating delayed cut efficiency:
  p.makeTool<MultCutTool>(TAG_LOW_DELAYED_EMIN);

  p.connect({inFile});

  // IbdSel and MultCut need to be "connected" before we modify their cuts,
  // otherwise our changes will be overwritten
  for (const Det detector : allADs) {
    const int tag = IbdSel::TAG(detector, TAG_LOW_DELAYED_EMIN);
    auto* sel = p.getAlg<IbdSel>(tag);
    sel->DELAYED_MIN = LOW_DELAYED_EMIN;
  }

  auto* mc = p.getTool<MultCutTool>(TAG_LOW_DELAYED_EMIN);
  mc->ibdCuts.emin_after = LOW_DELAYED_EMIN;
  mc->singleCuts.emin_after = LOW_DELAYED_EMIN;

  p.loop();

  Calculator calc(p, site, phase, seq);
  calc.writeEntries();
}

