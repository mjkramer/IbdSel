// Force assertion checks, even in non-debug builds
#undef NDEBUG
#include <cassert>

#include "AdSaver.cc"
#include "EventReader.cc"
#include "FlasherCut.cc"
#include "LivetimeSaver.cc"
#include "MuonSaver.cc"
#include "TrigTypeCut.cc"

#include "../common/common_deps.cc"

#include <string>
#include <vector>

void stage1_main(const char* inFile, const char* outFile, Site site, Phase phase)
{
  Pipeline p;

  p.makeOutFile(outFile);

  p.makeAlg<EventReader>();
  p.makeAlg<TrigTypeCut>();
  p.makeAlg<MuonSaver>();
  p.makeAlg<FlasherCut>();

  for (Det detector : util::ADsFor(site, phase)) {
    p.makeAlg<AdSaver>(detector);
  }

  p.makeAlg<LivetimeSaver>();

  p.process({inFile});
}
