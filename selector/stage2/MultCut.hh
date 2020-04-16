#pragma once

#include "AdBuffer.hh"
#include "MuonAlg.hh"

#include "../common/Constants.hh"

class MultCutTool : public Tool {
public:
  static constexpr float PROMPT_MIN = 0.7;
  static constexpr float DELAYED_MIN = 6;
  static constexpr float DELAYED_MAX = 12;
  static constexpr int IBD_USEC_BEFORE = 400;
  static constexpr int IBD_USEC_AFTER = 200;
  static constexpr int SINGLE_USEC_BEFORE = 400;
  static constexpr int SINGLE_USEC_AFTER = 200;

  using Iter = AdBuffer::Iter;

  void connect(Pipeline& pipeline) override;

  bool ibdDmcOk(Iter itP, Iter itD, Det det) const;
  bool singleDmcOk(Iter it, Det det) const;

  // Cuts should be private, but then the _dict.cxx generated by rootcling will
  // still try to access it (because we enabled the nestedclasses pragma?),
  // causing a compiler error. Maybe we should report this as a ROOT bug. (Why
  // doesn't ACLiC fail?)
  struct Cuts {
    float usec_before, usec_after, emin_before, emin_after, emax_after;
  };


private:
  bool dmcOk(std::optional<Iter> optItP,
             Iter itD,
             Det det,
             Cuts cuts,
             const MuonAlg* muonAlg) const;

  const MuonAlg* muonAlgIBDs;
  const MuonAlg* muonAlgSingles;
};

inline bool MultCutTool::ibdDmcOk(Iter itP, Iter itD, Det det) const
{
  Cuts cuts {IBD_USEC_BEFORE, IBD_USEC_AFTER, PROMPT_MIN, DELAYED_MIN,
    DELAYED_MAX};
  return dmcOk(itP, itD, det, cuts, muonAlgIBDs);
}

inline bool MultCutTool::singleDmcOk(Iter it, Det det) const
{
  Cuts cuts {SINGLE_USEC_BEFORE, SINGLE_USEC_AFTER, PROMPT_MIN, DELAYED_MIN,
    DELAYED_MAX};
  return dmcOk(std::nullopt, it, det, cuts, muonAlgSingles);
}
