#pragma once

#include <vector>
#include <cassert>

#include "Readers.cc"
#include "MuonAlg.cc"

using Data = ClusterTree;

class MultCutTool : public Tool {
  static constexpr float PROMPT_MIN = 0.7;
  static constexpr float DELAYED_MIN = 6;

public:
  void connect(Pipeline& pipeline) override;

  bool pairDmcOk(const Data& cluster, int detector,
                 size_t iP, size_t iD) const;
  bool singleDmcOk(const Data& cluster, int detector,
                   size_t i) const;

private:
  const MuonAlg* muonAlg;
};

void MultCutTool::connect(Pipeline& pipeline)
{
  muonAlg = pipeline.getAlg<MuonAlg>();
  assert(muonAlg->getTag() == MuonAlg::Purpose::ForIBDs);
}

bool MultCutTool::pairDmcOk(const Data& cluster, int detector,
                            size_t iP, size_t iD) const
{
  for (size_t iX = 0; iX < cluster.size; ++iX) {
    if (iX == iP || iX == iD)
      continue;

    const float dt_us = cluster.time(iD).diff_us(cluster.time(iX));
    const float eX = cluster.energy[iX];

    // Note: We don't apply an upper cut on energy of "extra" event
    // This avoids introducing the double-neutron background

    if (0 < dt_us && dt_us < 400 &&  // before delayed
        PROMPT_MIN < eX)
      return false;

    if (-200 < dt_us && dt_us < 0 && // after delayed
        DELAYED_MIN < eX &&
        !muonAlg->isVetoed(cluster.time(iX), detector))
      return false;
  }

  return true;
}

// Do we need this?
bool MultCutTool::singleDmcOk(const Data& cluster, int detector,
                               size_t i) const
{
  return pairDmcOk(cluster, detector, -1, i);
}
