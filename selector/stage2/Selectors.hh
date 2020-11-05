#pragma once

#include "AdBuffer.hh"
#include "IbdTree.hh"
#include "MuonAlg.hh"

#include "TreeWriter.hh"

#include <cfloat>               // FLT_MAX

class MultCutTool;
class TH1F;
class Config;

class SelectorBase : public BufferedSimpleAlg<AdBuffer, Det> {
public:
  using Iter = AdBuffer::Iter;

  SelectorBase(Det det, MuonAlg::Purpose purpose);

  void connect(Pipeline& p) override;
  Algorithm::Status consume_iter(Iter it) final;
  const AdTree& getCurrent() const // for logging purposes
  {
    return *current;
  }

  virtual void initCuts(const Config* config) = 0;
  virtual void select(Iter it) = 0;

  const Det det;

protected:
  const MuonAlg* muonAlg;
  const MultCutTool* multCut;

private:
  const MuonAlg::Purpose purpose;
  Iter current;
};

// -----------------------------------------------------------------------------

class SingleSel : public SelectorBase {
public:
  float EMIN;
  float EMAX;

  SingleSel(Det det);
  void initCuts(const Config* config) override;
  void select(Iter it) override;
  void finalize(Pipeline& p) override;

  TH1F* hist;
};

// -----------------------------------------------------------------------------

class IbdSel : public SelectorBase {
public:
  float PROMPT_MIN;
  float PROMPT_MAX;
  float DELAYED_MIN;
  float DELAYED_MAX;
  unsigned DT_MIN_US;
  unsigned DT_MAX_US;

  IbdSel(Det detector);
  void connect(Pipeline& p) override;
  void initCuts(const Config* config) override;
  void select(Iter it) override;
  void finalize(Pipeline& p) override;

  TH1F* hist;

private:
  void save(Iter prompt, Iter delayed);

  TreeWriter<IbdTree> ibdTree;
};
