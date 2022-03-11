#pragma once

#include "Readers.hh"

#include "../SelectorFramework/core/ConfigTool.hh"
#include "../SelectorFramework/core/RingBuf.hh"
#include "../SelectorFramework/core/SimpleAlg.hh"

class MuonAlg : public SimpleAlg<MuonReader> {
  static constexpr unsigned BUF_SIZE = 100'000;

public:
  enum class Purpose { ForIBDs, ForSingles };

  MuonAlg(Purpose purp) :
    purpose(purp), muonBuf(BUF_SIZE) {}

  // So we can select MuonAlg by its purpose
  int rawTag() const override { return int(purpose); }

  void connect(Pipeline& p) override;
  Algorithm::Status consume(const MuonTree& e) override;

  bool isVetoed(Time t, Det detector) const;
  double vetoTime_s(Det detector) const;

  void log(const char* msg, Det det, Time t,
           const MuonTree* muon = nullptr) const;

private:
  void initCuts(const Config* config);
  Time endOfLastVeto(size_t idet) const;
  bool isWP(const MuonTree& e) const;
  bool isShower(const MuonTree& e) const;
  bool isAD(const MuonTree& e) const;
  bool isExtraIws(const MuonTree& e) const;
  void log(const MuonTree& muon, const char* msg);
  float nomPostVeto_us(const MuonTree& e) const;
  float effVeto_us(const MuonTree& e, size_t idet) const;

  const Purpose purpose;

  double muPreVeto_us;

  int wpMuNhitCut;
  double wpMuPostVeto_us;

  double adMuChgCut;
  double adMuPostVeto_us;

  double showerMuChgCut;
  double showerMuPostVeto_us;

  bool extraIwsVeto;
  int extraIwsNhitAtLeast;
  float extraIwsPostVeto_us;

  bool vetoAroundGaps = false;

  Time lastWpTime;
  Time lastAdTime[4];
  Time lastShowerTime[4];
  Time lastExtraIwsTime;

  RingBuf<MuonTree> muonBuf;
  double vetoTime_s_[4] = {0};

  mutable size_t lastBufDepth[4] = {0}; // optimization
};
