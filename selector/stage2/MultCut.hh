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
  static constexpr int SINGLE_USEC_BEFORE = 1000;
  static constexpr int SINGLE_USEC_AFTER = SINGLE_USEC_BEFORE;

  using Iter = AdBuffer::Iter;

  void connect(Pipeline& pipeline) override;

  bool ibdDmcOk(Iter itP, Iter itD, Det det) const;
  bool singleDmcOk(Iter it, Det det) const;

private:
  struct Cuts {
    float usec_before, usec_after, emin_before, emin_after, emax_after;
  };

  bool dmcOk(std::optional<Iter> optItP,
             Iter itD,
             Det det,
             Cuts cuts,
             const MuonAlg* muonAlg) const;

  const MuonAlg* muonAlgIBDs;
  const MuonAlg* muonAlgSingles;
};
