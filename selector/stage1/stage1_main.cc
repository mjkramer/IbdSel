#include "stage1_main.hh"

#include "AdSaver.hh"
#include "EventReader.hh"
#include "FlasherCut.hh"
#include "LivetimeSaver.hh"
#include "MuonSaver.hh"
#include "TrigTypeCut.hh"

#include <string>
#include <vector>

#include "../common/Misc.hh"

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
