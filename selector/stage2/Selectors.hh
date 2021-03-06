#pragma once

#include "AdBuffer.hh"
#include "IbdTree.hh"
#include "MuonAlg.hh"

#include "../SelectorFramework/core/TreeWriter.hh"

#include <cfloat>               // FLT_MAX

class MultCutTool;
class VertexCutTool;
class TH1F;
class Config;

class SelectorBase : public BufferedSimpleAlg<AdBuffer> {
public:
  using Iter = AdBuffer::Iter;

  // lowercase tag is used to select the MultCut
  static int TAG(Det det, int tag = 0)
  {
    return (tag << 8) + int(det);
  }

  SelectorBase(Det det, MuonAlg::Purpose muonAlgPurpose, int tag);

  void connect(Pipeline& p) override;
  Algorithm::Status consume_iter(Iter it) final;
  const AdTree& getCurrent() const // for logging muonAlgPurposes
  {
    return *current;
  }

  virtual void initCuts(const Config* config) = 0;
  virtual void select(Iter it) = 0;

  const Det det;

  const MultCutTool* multCut;

protected:
  const MuonAlg* muonAlg;
  const VertexCutTool* vertexCut;
  const int multCutTag;

private:
  const MuonAlg::Purpose muonAlgPurpose;
  Iter current;
};

// -----------------------------------------------------------------------------

class SingleSel : public SelectorBase {
public:
  float EMIN;
  float EMAX;

  SingleSel(Det det, int multCutTag = 0);
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

  IbdSel(Det detector, int multCutTag = 0);
  void connect(Pipeline& p) override;
  void initCuts(const Config* config) override;
  void select(Iter it) override;
  void finalize(Pipeline& p) override;

  TH1F *histP, *histD;

private:
  void save(Iter prompt, Iter delayed);

  TreeWriter<IbdTree> ibdTree;
};
