#include "MultCut.hh"
#include "VertexCut.hh"

#include "../SelectorFramework/core/Assert.hh"

#include <limits>

void MultCutTool::connect(Pipeline& pipeline)
{
  auto config = pipeline.getTool<Config>();
  initCuts(config);

  muonAlgIBDs = pipeline.getAlg<MuonAlg>(MuonAlg::Purpose::ForIBDs);
  ASSERT(muonAlgIBDs->rawTag() == int(MuonAlg::Purpose::ForIBDs));
  muonAlgSingles = pipeline.getAlg<MuonAlg>(MuonAlg::Purpose::ForSingles);
  ASSERT(muonAlgSingles->rawTag() == int(MuonAlg::Purpose::ForSingles));

  vertexCut = pipeline.getTool<VertexCutTool>();
}

void MultCutTool::initCuts(const Config* config)
{
  auto g = [&](const char* k) { return config->get<float>(k); };

  ibdCuts.usec_before = g("ibdDmcUsecBefore");
  ibdCuts.emin_before = g("ibdDmcEminBefore");
  ibdCuts.emax_before = g("ibdDmcEmaxBefore");

  ibdCuts.usec_after = g("ibdDmcUsecAfter");
  ibdCuts.emin_after = g("ibdDmcEminAfter");
  ibdCuts.emax_after = g("ibdDmcEmaxAfter");

  singleCuts.usec_before = g("singleDmcUsecBefore");
  singleCuts.emin_before = g("singleDmcEminBefore");
  singleCuts.emax_before = g("singleDmcEmaxBefore");

  singleCuts.usec_after = g("singleDmcUsecAfter");
  singleCuts.emin_after = g("singleDmcEminAfter");
  singleCuts.emax_after = g("singleDmcEmaxAfter");
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

    if (other->energy > cuts.emin_before &&
        other->energy < cuts.emax_before &&
        vertexCut->vertexOk(other)) {
      return false;
    }
  }

  for (Iter other = itD.later();
       other->time().diff_us(itD->time()) < cuts.usec_after;
       other = other.later()) {

    if (other->energy > cuts.emin_after &&
        other->energy < cuts.emax_after &&
        // XXX remove the following if we restore the "full DMC" 200us pre-veto
        not muonAlg->isVetoed(other->time(), det) &&
        vertexCut->vertexOk(other)) {

      return false;
    }
  }

  return true;
}
