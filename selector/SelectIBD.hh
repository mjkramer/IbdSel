#pragma once

#include <iostream>

#include "ClusterAlg.hh"
#include "MuonAlg.hh"
#include "MultCut.hh"

using Status = Algorithm::Status;
using Data = EventReader::Data;

class SelectIBD : public Algorithm {
  static constexpr float PROMPT_MIN = 0.7;
  static constexpr float PROMPT_MAX = 12;
  static constexpr float DELAYED_MIN = 6;
  static constexpr float DELAYED_MAX = 12;

public:
  SelectIBD(int detector) : detector(detector) {}

  void connect(Pipeline& pipeline) override;
  Status execute() override;

private:
  void process(const Data& prompt, const Data& delayed);

  const short detector;

  const ClusterAlg* clusterAlg;
  const MuonAlg* muonAlg;
  const MultCutTool* multCutTool;

  TFile* outFile;
};

inline void SelectIBD::connect(Pipeline& pipeline)
{
  auto pred = [this](const ClusterAlg& alg) {
    return alg.detector == detector;
  };
  clusterAlg = pipeline.getAlg<ClusterAlg>(pred);

  muonAlg = pipeline.getAlg<MuonAlg>();

  multCutTool = pipeline.getTool<MultCutTool>();

  outFile = pipeline.getOutFile();
}

inline Status SelectIBD::execute()
{
  if (!clusterAlg->ready())
    return Status::Continue;

  const std::vector<Data>& cluster = clusterAlg->getCluster();

  for (size_t iP = 0; iP < cluster.size() - 1; ++iP) {
    for (size_t iD = iP+1; iD < cluster.size(); ++iD) {
      const float eP = cluster[iP].energy;
      const float eD = cluster[iD].energy;
      const float dt_us = cluster[iD].time().diff_us(cluster[iP].time());

      if (PROMPT_MIN  < eP && eP < PROMPT_MAX &&
          DELAYED_MIN < eD && eD < DELAYED_MAX &&
          1 < dt_us && dt_us < 200 &&
          !muonAlg->isVetoed(cluster[iD]) &&
          multCutTool->pairDmcOk(cluster, iP, iD)) {
        // Got one!
        process(cluster[iP], cluster[iD]);
        // return Status::Continue;
      }
    }
  }

  return Status::Continue;
}

inline void SelectIBD::process(const Data& prompt, const Data& delayed)
{
  std::cout << detector << " "
            << prompt.triggerNumber << " "
            << delayed.triggerNumber << std::endl;
}
