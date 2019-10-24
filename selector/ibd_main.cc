#include "ibd_main.hh"

#include "EventReader.hh"
#include "MuonAlg.hh"
#include "ClusterAlg.hh"
#include "SelectIBD.hh"
#include "SelectSingles.hh"

#include "TrigTypeCut.hh"
#include "FlasherCut.hh"

void ibd_main(const std::vector<std::string>& inFiles, const char* outFile)
{
  Pipeline p;

  p.makeOutFile(outFile);

  p.makeAlg<EventReader>();
  p.makeAlg<TrigTypeCut>();
  p.makeAlg<MuonAlg>();
  p.makeAlg<FlasherCut>();

  for (int detector = 1; detector <= 4; ++detector) {
    p.makeAlg<ClusterAlg>(detector);
    p.makeAlg<SelectIBD>(detector);
    p.makeAlg<SelectSingles>(detector);
  }

  p.makeTool<MultCutTool>();

  p.process(inFiles);
}
