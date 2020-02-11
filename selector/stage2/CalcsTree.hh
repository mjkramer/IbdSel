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

  UInt_t seq;
  Phase phase;
  Site site;
  Det detector;

  // XXX diagnostics
  Float_t nPlusLikeSingles, nPromptLikeSingles, nDelayedLikeSingles;
  Float_t promptLikeHz, delayedLikeHz, dmcEffSingles;

  void initBranches() override;
};

