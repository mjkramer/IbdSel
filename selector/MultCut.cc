#pragma once

#include <cassert>

#include "Constants.cc"
#include "AdBuffer.cc"
#include "MuonAlg.cc"

class MultCutTool : public Tool {
  static constexpr float PROMPT_MIN = 0.7;
  static constexpr float DELAYED_MIN = 6;
  static constexpr int IBD_USEC_BEFORE = 400;
  static constexpr int IBD_USEC_AFTER = 200;
  static constexpr int SINGLE_USEC_BEFORE = 1000;
  static constexpr int SINGLE_USEC_AFTER = SINGLE_USEC_BEFORE;

  using Iter = AdBuffer::Iter;

public:
  void connect(Pipeline& pipeline) override;

  bool ibdDmcOk(Iter itP, Iter itD, Det det) const;
  bool singleDmcOk(Iter it, Det det) const;

private:
  struct Cuts {
    float usec_before, usec_after, emin_before, emin_after;
  };

  bool dmcOk(std::optional<Iter> optItP,
             Iter itD,
             Det det,
             Cuts cuts) const;

  const MuonAlg* muonAlg;
};

void MultCutTool::connect(Pipeline& pipeline)
{
  muonAlg = pipeline.getAlg<MuonAlg>(MuonAlg::Purpose::ForIBDs);
  assert(muonAlg->rawTag() == int(MuonAlg::Purpose::ForIBDs));
}

bool MultCutTool::dmcOk(std::optional<Iter> optItP,
                        Iter itD,
                        Det det,
                        Cuts cuts) const
{
  for (Iter other = itD.earlier();
       itD->time().diff_us(other->time()) < cuts.usec_before;
       other = other.earlier()) {

    if (optItP && other == *optItP)
      continue;

    if (other->energy > cuts.emin_after) {
      return false;
    }
  }

  for (Iter other = itD.later();
       other->time().diff_us(itD->time()) < cuts.usec_after;
       other = other.later()) {

    if (other->energy > cuts.emin_after
        // XXX remove the following if we restore the "full DMC" 200us pre-veto
        && not muonAlg->isVetoed(other->time(), det)) {

      return false;
    }
  }

  return true;
}

inline bool MultCutTool::ibdDmcOk(Iter itP, Iter itD, Det det) const
{
  Cuts cuts {IBD_USEC_BEFORE, IBD_USEC_AFTER, PROMPT_MIN, DELAYED_MIN};
  return dmcOk(itP, itD, det, cuts);
}

inline bool MultCutTool::singleDmcOk(Iter it, Det det) const
{
  Cuts cuts {SINGLE_USEC_BEFORE, SINGLE_USEC_AFTER, PROMPT_MIN, PROMPT_MIN};
  return dmcOk(std::nullopt, it, det, cuts);
}
