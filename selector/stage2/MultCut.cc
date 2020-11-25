#include "MultCut.hh"

#include "Assert.hh"

#include <limits>

void MultCutTool::connect(Pipeline& pipeline)
{
  auto config = pipeline.getTool<Config>();
  initCuts(config);

  muonAlgIBDs = pipeline.getAlg<MuonAlg>(MuonAlg::Purpose::ForIBDs);
  ASSERT(muonAlgIBDs->rawTag() == int(MuonAlg::Purpose::ForIBDs));
  muonAlgSingles = pipeline.getAlg<MuonAlg>(MuonAlg::Purpose::ForSingles);
  ASSERT(muonAlgSingles->rawTag() == int(MuonAlg::Purpose::ForSingles));
}

bool MultCutToolBcw::dmcOk(std::optional<Iter> optItP,
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
        other->energy < cuts.emax_before) {
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


void MultCutToolBcw::initCuts(const Config* config)
{
  auto g = [&](const char* k) { return config->get<float>(k); };

  ibdCuts.usec_before = g("ibdDmcBcwUsecBefore");
  ibdCuts.emin_before = g("ibdDmcBcwEminBefore");
  ibdCuts.emax_before = g("ibdDmcBcwEmaxBefore");

  ibdCuts.usec_after = g("ibdDmcBcwUsecAfter");
  ibdCuts.emin_after = g("ibdDmcBcwEminAfter");
  ibdCuts.emax_after = g("ibdDmcBcwEmaxAfter");

  singleCuts.usec_before = g("singleDmcBcwUsecBefore");
  singleCuts.emin_before = g("singleDmcBcwEminBefore");
  singleCuts.emax_before = g("singleDmcBcwEmaxBefore");

  singleCuts.usec_after = g("singleDmcBcwUsecAfter");
  singleCuts.emin_after = g("singleDmcBcwEminAfter");
  singleCuts.emax_after = g("singleDmcBcwEmaxAfter");
}

bool MultCutToolIhep::dmcOk(std::optional<Iter> optItP,
                        Iter itD,
                        Det det,
                        Cuts cuts,
                        const MuonAlg* muonAlg) const
{
  const float dt_PD = optItP ? itD->time().diff_us(optItP.value()->time())
                            : 0.;
  for (Iter other = itD.earlier();
      itD->time().diff_us(other->time()) < cuts.usec_before + dt_PD; // 200 us + dt
      other = other.earlier()) {

    if (optItP && other == *optItP)
      continue;
    
    if (other->energy > cuts.emin_before &&
        other->energy < cuts.emax_before) {
      return false;
    }
  }

  for (Iter other = itD.later(); 
      other->time().diff_us(itD->time()) < cuts.usec_after;
      other = other.later()) {
    
    if (other->energy > cuts.emin_after && 
        other->energy < cuts.emax_after &&
        not muonAlg->isVetoed(other->time(), det)) { 
      
      return false;
    } 
  }

  return true;
}

void MultCutToolIhep::initCuts(const Config* config)
{
  auto g = [&](const char* k) { return config->get<float>(k); };

  ibdCuts.usec_before = g("ibdDmcIhepUsecBefore");
  ibdCuts.emin_before = g("ibdDmcIhepEminBefore");
  ibdCuts.emax_before = g("ibdDmcIhepEmaxBefore");

  ibdCuts.usec_after = g("ibdDmcIhepUsecAfter");
  ibdCuts.emin_after = g("ibdDmcIhepEminAfter");
  ibdCuts.emax_after = g("ibdDmcIhepEmaxAfter");

  singleCuts.usec_before = g("singleDmcIhepUsecBefore");
  singleCuts.emin_before = g("singleDmcIhepEminBefore");
  singleCuts.emax_before = g("singleDmcIhepEmaxBefore");

  singleCuts.usec_after = g("singleDmcIhepUsecAfter");
  singleCuts.emin_after = g("singleDmcIhepEminAfter");
  singleCuts.emax_after = g("singleDmcIhepEmaxAfter");
}
