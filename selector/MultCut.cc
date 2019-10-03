#pragma once

#include <vector>

#include "EventReader.cc"
#include "MuonAlg.cc"

using Data = EventReader::Data;

class MultCutTool : public Tool {
  static constexpr float PROMPT_MIN = 0.7;
  static constexpr float DELAYED_MIN = 6;

public:
  void connect(Pipeline& pipeline) override;

  bool pairDmcOk(const std::vector<Data>& cluster, size_t iP, size_t iD) const;
  bool singleDmcOk(const std::vector<Data>& cluster, size_t i) const;

private:
  const MuonAlg* muonAlg;
};

void MultCutTool::connect(Pipeline& pipeline)
{
  muonAlg = pipeline.getAlg<MuonAlg>();
}

bool MultCutTool::pairDmcOk(const std::vector<Data>& cluster,
                            size_t iP, size_t iD) const
{
  for (size_t iX = 0; iX < cluster.size(); ++iX) {
    if (iX == iP || iX == iD)
      continue;

    const float dt_us = cluster[iD].time().diff_us(cluster[iX].time());
    const float eX = cluster[iX].energy;

    // Note: We don't apply an upper cut on energy of "extra" event
    // This avoids introducing the double-neutron background

    if (0 < dt_us && dt_us < 400 &&  // before delayed
        PROMPT_MIN < eX)
      return false;

    if (-200 < dt_us && dt_us < 0 && // after delayed
        DELAYED_MIN < eX &&
        !muonAlg->isVetoed(cluster[iX]))
      return false;
  }

  return true;
}

bool MultCutTool::singleDmcOk(const std::vector<Data>& cluster,
                               size_t i) const
{
  return pairDmcOk(cluster, -1, i);
}

