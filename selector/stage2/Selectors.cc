#pragma once

#include "AdBuffer.cc"
#include "IbdTree.cc"
#include "MuonAlg.cc"
#include "MultCut.cc"

#include "../common/Constants.cc"

#include "../SelectorFramework/core/TreeWriter.cc"

#include <TH1F.h>

class SelectorBase : public BufferedSimpleAlg<AdBuffer, Det> {
public:
  using Iter = AdBuffer::Iter;

  SelectorBase(Det det, MuonAlg::Purpose purpose) :
    BufferedSimpleAlg<AdBuffer, Det>(det),
    det(det),
    purpose(purpose) {}

  void connect(Pipeline& p) override;

  const Det det;

protected:
  const MuonAlg* muonAlg;
  const MultCutTool* multCut;

private:
  const MuonAlg::Purpose purpose;
};

void SelectorBase::connect(Pipeline &p)
{
  BufferedSimpleAlg<AdBuffer, Det>::connect(p);

  muonAlg = p.getAlg<MuonAlg>(purpose);
  assert(muonAlg->rawTag() == int(purpose));

  multCut = p.getTool<MultCutTool>();
}

// ----------------------------------------------------------------------

class SingleSel : public SelectorBase {
public:
  static constexpr float EMIN = 0.7;
  static constexpr float EMAX = 12;

  SingleSel(Det det);
  Algorithm::Status consume_iter(Iter it) override;
  void finalize(Pipeline& p) override;

  TH1F* hist;
};

SingleSel::SingleSel(Det det) :
  SelectorBase(det, MuonAlg::Purpose::ForSingles)
{
  auto hname = Form("h_single_d%d", int(det));
  hist = new TH1F(hname, hname, 113, 0.7, 12);
}

void SingleSel::finalize(Pipeline& _p)
{
  hist->Write();
}

Algorithm::Status SingleSel::consume_iter(Iter it)
{
  if (EMIN < it->energy && it->energy < EMAX &&
      not muonAlg->isVetoed(it->time(), det) &&
      multCut->singleDmcOk(it, det)) {

    hist->Fill(it->energy);
  }

  return Status::Continue;
}

// ----------------------------------------------------------------------

class IbdSel : public SelectorBase {
public:
  static constexpr float PROMPT_MIN = 0.7;
  static constexpr float PROMPT_MAX = 12;
  static constexpr float DELAYED_MIN = 6;
  static constexpr float DELAYED_MAX = 12;
  static constexpr unsigned DT_MIN_US = 1;
  static constexpr unsigned DT_MAX_US = 200;

  IbdSel(Det detector);
  void connect(Pipeline& p) override;
  Algorithm::Status consume_iter(Iter it) override;
  void finalize(Pipeline& p) override;

  TH1F* hist;

private:
  void save(Iter prompt, Iter delayed);

  TreeWriter<IbdTree> ibdTree;
};

IbdSel::IbdSel(Det detector) :
  SelectorBase(detector, MuonAlg::Purpose::ForIBDs),
  ibdTree(Form("ibd_AD%d", int(detector)))
{
  auto hname = Form("h_ibd_d%d", detector);
  hist = new TH1F(hname, hname, 113, 0.7, 12);
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

  ibdTree.fill();
}

Algorithm::Status IbdSel::consume_iter(Iter it)
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

  return Status::Continue;
}
