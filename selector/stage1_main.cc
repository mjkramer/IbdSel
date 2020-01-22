#include <string>
#include <vector>

#include "EventReader.cc"
#include "TrigTypeCut.cc"
#include "MuonSaver.cc"
#include "FlasherCut.cc"
#include "AdSaver.cc"
#include "LivetimeSaver.cc"
#include "Misc.cc"

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
