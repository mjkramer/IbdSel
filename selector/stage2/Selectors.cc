#include "Selectors.hh"
#include "MultCut.hh"
#include "MuonAlg.hh"

#include "../SelectorFramework/core/Assert.hh"
#include "../SelectorFramework/core/ConfigTool.hh"

#include <TH1F.h>

SelectorBase::SelectorBase(Det det, MuonAlg::Purpose purpose) :
  BufferedSimpleAlg<AdBuffer, Det>(det),
  det(det),
  purpose(purpose) {}

void SelectorBase::connect(Pipeline &p)
{
  BufferedSimpleAlg<AdBuffer, Det>::connect(p);

  muonAlg = p.getAlg<MuonAlg>(purpose);
  ASSERT(muonAlg->rawTag() == int(purpose));

  multCut = p.getTool<MultCutTool>();

  const Config* config = p.getTool<Config>();
  initCuts(config);
}

inline
Algorithm::Status SelectorBase::consume_iter(Iter it)
{
  current = it;
  select(it);
  return Status::Continue;
}

// ----------------------------------------------------------------------

SingleSel::SingleSel(Det det) :
  SelectorBase(det, MuonAlg::Purpose::ForSingles)
{
  auto hname = Form("h_single_AD%d", int(det));
  // Use fine binning since we're doing integrals in Calculator
  // Go up to 20 MeV so we can calc pre-muon rate (for debugging)
  hist = new TH1F(hname, hname, 4000, 0, 20);
}

void SingleSel::initCuts(const Config *config)
{
  EMIN = config->get<float>("singleEmin", 0.7);
  // No upper limit (we want premuons):
  EMAX = config->get<float>("singleEmax", 99999);
}

void SingleSel::finalize(Pipeline& _p)
{
  hist->Write();
}

void SingleSel::select(Iter it)
{
  if (EMIN < it->energy && it->energy < EMAX &&
      not muonAlg->isVetoed(it->time(), det) &&
      multCut->singleDmcOk(it, det)) {

    if (it->z > 0)
      return;

    hist->Fill(it->energy);
  }
}

// ----------------------------------------------------------------------

IbdSel::IbdSel(Det detector) :
  SelectorBase(detector, MuonAlg::Purpose::ForIBDs),
  ibdTree(Form("ibd_AD%d", int(detector)))
{
}

void IbdSel::connect(Pipeline& p)
{
  ibdTree.connect(p);
  SelectorBase::connect(p);
}

void IbdSel::initCuts(const Config *config)
{
  PROMPT_MIN = config->get<float>("ibdPromptEmin", 0.7);
  PROMPT_MAX = config->get<float>("ibdPromptEmax", 12);
  DELAYED_MIN = config->get<float>("ibdDelayedEmin", 6);
  DELAYED_MAX = config->get<float>("ibdDelayedEmax", 12);
  DT_MIN_US = config->get<float>("ibdDtMinUsec", 1);
  DT_MAX_US = config->get<float>("ibdDtMaxUsec", 200);

  auto hname = Form("h_ibd_AD%d", det);
  hist = new TH1F(hname, hname, 20 * PROMPT_MAX, 0, PROMPT_MAX);
}

void IbdSel::finalize(Pipeline& p)
{
  hist->Write();
}

void IbdSel::save(Iter prompt, Iter delayed)
{
  ibdTree.data.runNo = delayed->runNo;
  ibdTree.data.fileNo = delayed->fileNo;
  ibdTree.data.trigP = prompt->trigNo;
  ibdTree.data.trigD = delayed->trigNo;
  ibdTree.data.eP = prompt->energy;
  ibdTree.data.eD = delayed->energy;
  ibdTree.data.dt_us = delayed->time().diff_us(prompt->time());

  ibdTree.fill();
}

void IbdSel::select(Iter it)
{
  auto dt_us = [&](Iter other) { return it->time().diff_us(other->time()); };

  if (DELAYED_MIN < it->energy && it->energy < DELAYED_MAX &&
      not muonAlg->isVetoed(it->time(), det)) {

    for (Iter prompt = it.earlier();
         dt_us(prompt) < DT_MAX_US;
         prompt = prompt.earlier()) {

      if (PROMPT_MIN < prompt->energy && prompt->energy < PROMPT_MAX &&
          dt_us(prompt) > DT_MIN_US &&
          multCut->ibdDmcOk(prompt, it, det)) {

        if (prompt->z > 0)
          continue;

        save(prompt, it);

        hist->Fill(prompt->energy);
      }
    }
  }
}
