#include <string>
#include <vector>

#include "EventReader.cc"
#include "TrigTypeCut.cc"
#include "MuonAlg.cc"
#include "FlasherCut.cc"
#include "ClusterAlg.cc"
#include "SelectIBD.cc"
#include "SelectSingles.cc"

const std::vector<std::string> deffiles = {
 "/global/projecta/projectdirs/dayabay/data/dropbox/p17b/lz3.skim.6/recon.Neutrino.0021221.Physics.EH1-Merged.P17B-P._0001.root"
};

void ibd_main(const std::vector<std::string>& inFiles = deffiles,
              const char* outFile = "results.root")
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
