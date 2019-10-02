#include <string>
#include <vector>

#include "EventReader.cc"
#include "TrigTypeCut.cc"
#include "MuonSaver.cc"
#include "FlasherCut.cc"
#include "ClusterSaver.cc"

const std::vector<std::string> deffiles = {
  "/global/projecta/projectdirs/dayabay/data/dropbox/p17b/lz3.skim.6/recon.Neutrino.0021221.Physics.EH1-Merged.P17B-P._0001.root"
};

void stage1_main(int maxEvts=0, const std::vector<std::string>& inFiles = deffiles,
                 const char* outFile = "results.root")
{
  Pipeline p;

  p.makeOutFile(outFile);

  p.makeAlg<EventReader>().setMaxEvents(maxEvts);
  p.makeAlg<TrigTypeCut>();
  p.makeAlg<MuonSaver>();
  p.makeAlg<FlasherCut>();

  for (int detector = 1; detector <= 4; ++detector) {
    p.makeAlg<ClusterSaver>(detector);
  }

  p.process(inFiles);
}
