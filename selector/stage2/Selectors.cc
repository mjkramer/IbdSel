#include "Selectors.hh"
#include "MultCut.hh"
#include "MuonAlg.hh"

#include "../SelectorFramework/core/Assert.hh"

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
  hist = new TH1F(hname, hname, 240, 0, 12);
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

    hist->Fill(it->energy);
  }
}

// ----------------------------------------------------------------------

IbdSel::IbdSel(Det detector) :
  SelectorBase(detector, MuonAlg::Purpose::ForIBDs),
  ibdTree(Form("ibd_AD%d", int(detector)))
{
  auto hname = Form("h_ibd_AD%d", detector);
  hist = new TH1F(hname, hname, 240, 0, 12);
}

void IbdSel::connect(Pipeline& p)
{
  ibdTree.connect(p);
  SelectorBase::connect(p);
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

        save(prompt, it);

        hist->Fill(prompt->energy);
      }
    }
  }
}
