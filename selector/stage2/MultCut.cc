#pragma once

#include "AdBuffer.cc"
#include "MuonAlg.cc"

#include "../common/Constants.cc"

#include <cassert>
#include <limits>

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

void MultCutTool::connect(Pipeline& pipeline)
{
  muonAlgIBDs = pipeline.getAlg<MuonAlg>(MuonAlg::Purpose::ForIBDs);
  assert(muonAlgIBDs->rawTag() == int(MuonAlg::Purpose::ForIBDs));
  muonAlgSingles = pipeline.getAlg<MuonAlg>(MuonAlg::Purpose::ForSingles);
  assert(muonAlgSingles->rawTag() == int(MuonAlg::Purpose::ForSingles));
}

bool MultCutTool::dmcOk(std::optional<Iter> optItP,
                        Iter itD,
                        Det det,
                        Cuts cuts,
                        const MuonAlg* muonAlg) const
{
  for (Iter other = itD.earlier();
       itD->time().diff_us(other->time()) < cuts.usec_before;
       other = other.earlier()) {

    if (optItP && other == *optItP)
      continue;

    if (other->energy > cuts.emin_before) {
      return false;
    }
  }

  for (Iter other = itD.later();
       other->time().diff_us(itD->time()) < cuts.usec_after;
       other = other.later()) {

    if (other->energy > cuts.emin_after &&
        other->energy < cuts.emax_after &&
        // XXX remove the following if we restore the "full DMC" 200us pre-veto
        not muonAlg->isVetoed(other->time(), det)) {

      return false;
    }
  }

  return true;
}

inline bool MultCutTool::ibdDmcOk(Iter itP, Iter itD, Det det) const
{
  Cuts cuts {IBD_USEC_BEFORE, IBD_USEC_AFTER, PROMPT_MIN, DELAYED_MIN,
    DELAYED_MAX};
  return dmcOk(itP, itD, det, cuts, muonAlgIBDs);
}

inline bool MultCutTool::singleDmcOk(Iter it, Det det) const
{
  Cuts cuts {SINGLE_USEC_BEFORE, SINGLE_USEC_AFTER, PROMPT_MIN, PROMPT_MIN,
    std::numeric_limits<float>::max()};
  return dmcOk(std::nullopt, it, det, cuts, muonAlgSingles);
}
