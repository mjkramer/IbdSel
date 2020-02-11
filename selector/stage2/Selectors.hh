#pragma once

#include "AdBuffer.hh"
#include "IbdTree.hh"
#include "MuonAlg.hh"

#include "../SelectorFramework/core/TreeWriter.hh"


class MultCutTool;
class TH1F;

class SelectorBase : public BufferedSimpleAlg<AdBuffer, Det> {
public:
  using Iter = AdBuffer::Iter;

  SelectorBase(Det det, MuonAlg::Purpose purpose);

  void connect(Pipeline& p) override;
  Algorithm::Status consume_iter(Iter it) final;
  const AdTree& getCurrent() const; // for logging purposes

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
  static constexpr float EMIN = 0.7;
  static constexpr float EMAX = 12;

  SingleSel(Det det);
  void select(Iter it) override;
  void finalize(Pipeline& p) override;

  TH1F* hist;
};

// -----------------------------------------------------------------------------

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
  void select(Iter it) override;
  void finalize(Pipeline& p) override;

  TH1F* hist;

private:
  void save(Iter prompt, Iter delayed);

  TreeWriter<IbdTree> ibdTree;
};
