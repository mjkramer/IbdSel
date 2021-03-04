#pragma once

#include "../common/Constants.hh"

#include "../SelectorFramework/core/BaseIO.hh"

class CalcsTree : public TreeBase {
public:
  Float_t livetime_s;
  Float_t vetoEff;
  Float_t dmcEff;

  Float_t accDaily, accDailyErr;
  Float_t li9Daily, li9DailyErr;

  Float_t delayedEffRel, delayedEffAbs;
  Float_t accDailyLowDelCut;

  UInt_t seq;
  Phase phase;
  Site site;
  Det detector;

  // For diagnostics and ReCalc
  Float_t nPreMuons, nPlusLikeSingles, nPromptLikeSingles, nDelayedLikeSingles;
  Float_t preMuonHz, plusLikeHz, promptLikeHz, delayedLikeHz;
  Float_t vetoEffSingles, dmcEffSingles;

  void initBranches() override;
};
