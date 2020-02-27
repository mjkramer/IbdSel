#include "MultCut.hh"

#include "../SelectorFramework/core/Assert.hh"

#include <limits>

void MultCutTool::connect(Pipeline& pipeline)
{
  muonAlgIBDs = pipeline.getAlg<MuonAlg>(MuonAlg::Purpose::ForIBDs);
  ASSERT(muonAlgIBDs->rawTag() == int(MuonAlg::Purpose::ForIBDs));
  muonAlgSingles = pipeline.getAlg<MuonAlg>(MuonAlg::Purpose::ForSingles);
  ASSERT(muonAlgSingles->rawTag() == int(MuonAlg::Purpose::ForSingles));
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
