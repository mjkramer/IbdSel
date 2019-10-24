#include <string>
#include <vector>

#include "stage1_main.hh"

#include "EventReader.hh"
#include "MuonSaver.hh"
#include "ClusterSaver.hh"

#include "TrigTypeCut.hh"
#include "FlasherCut.hh"

void stage1_main(const char* inFile, const char* outFile, int site)
{
  Pipeline p;

  p.makeOutFile(outFile);

  p.makeAlg<EventReader>();
  p.makeAlg<TrigTypeCut>();
  p.makeAlg<MuonSaver>();
  p.makeAlg<FlasherCut>();

  const int maxDet = site == 3 ? 4 : 2;

  for (int detector = 1; detector <= maxDet; ++detector) {
    p.makeAlg<ClusterSaver>(detector);
  }

  p.process({inFile});
}

