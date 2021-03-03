#pragma once

#include "AdBuffer.hh"
#include "MuonAlg.hh"

#include "../common/Constants.hh"

class VertexCutTool;

class MultCutTool : public Tool {
public:
  using Iter = AdBuffer::Iter;

  using Tool::Tool;

  void connect(Pipeline& pipeline) override;

  bool ibdDmcOk(Iter itP, Iter itD, Det det) const;
  bool singleDmcOk(Iter it, Det det) const;

  struct Cuts {
    float usec_before, emin_before, emax_before;
    float usec_after, emin_after, emax_after;
  };

  Cuts ibdCuts;
  Cuts singleCuts;

private:
  void initCuts(const Config* config);
  bool dmcOk(std::optional<Iter> optItP,
             Iter itD,
             Det det,
             Cuts cuts,
             const MuonAlg* muonAlg) const;

  const MuonAlg* muonAlgIBDs;
  const MuonAlg* muonAlgSingles;

  const VertexCutTool* vertexCut;
};

inline bool MultCutTool::ibdDmcOk(Iter itP, Iter itD, Det det) const
{
  return dmcOk(itP, itD, det, ibdCuts, muonAlgIBDs);
}

inline bool MultCutTool::singleDmcOk(Iter it, Det det) const
{
  return dmcOk(std::nullopt, it, det, singleCuts, muonAlgSingles);
}
