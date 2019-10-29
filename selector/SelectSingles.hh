#pragma once

#include <TH1F.h>

#include "ClusterAlg.hh"
#include "MuonAlg.hh"
#include "MultCut.hh"

using Status = Algorithm::Status;
using Data = EventReader::Data;

class SelectSingles : public Algorithm {
  static constexpr float EMIN = 0.7;
  static constexpr float EMAX = 12;

public:
  SelectSingles(int detector) : detector(detector) {}

  void connect(Pipeline& pipeline) override;
  Status execute() override;
  void finalize(Pipeline& pipeline) override;

private:
  void initHists();

  const short detector;

  const ClusterAlg* clusterAlg;
  const MuonAlg* muonAlg;
  const MultCutTool* multCutTool;

  TFile* outFile;
  TH1F* hist;
};

inline void SelectSingles::connect(Pipeline& pipeline)
{
  auto pred = [this](const ClusterAlg& alg) {
    return alg.detector == detector;
  };
  clusterAlg = pipeline.getAlg<ClusterAlg>(pred);

  muonAlg = pipeline.getAlg<MuonAlg>();

  multCutTool = pipeline.getTool<MultCutTool>();

  outFile = pipeline.getOutFile();

  initHists();
}

inline void SelectSingles::initHists()
{
  outFile->cd();

  const char* name = Form("h_singles_d%d", detector);
  hist = new TH1F(name, name, 113, 0.7, 12);
}

inline void SelectSingles::finalize(Pipeline& _pipeline)
{
  hist->Write();
}

inline Status SelectSingles::execute()
{
  if (!clusterAlg->ready())
    return Status::Continue;

  const std::vector<Data>& cluster = clusterAlg->getCluster();

  for (size_t i = 0; i < cluster.size(); ++i) {
    const float e = cluster[i].energy;

    if (EMIN < e && e < EMAX &&
        !muonAlg->isVetoed(cluster[i]) &&
        multCutTool->singleDmcOk(cluster, i)) {
      hist->Fill(e);
    }
  }

  return Status::Continue;
}
