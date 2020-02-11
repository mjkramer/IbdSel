#pragma once

#include "../common/Constants.cc"

#include "../SelectorFramework/core/BaseIO.cc"

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

void CalcsTree::initBranches()
{
  BR(livetime_s);
  BR(vetoEff);
  BR(dmcEff);

  BR(accDaily); BR(accDailyErr);
  BR(li9Daily); BR(li9DailyErr);

  BR(seq);
  BR(phase);
  BR(site);
  BR(detector);

  // XXX diagnostics
  BR(nPlusLikeSingles); BR(nPromptLikeSingles); BR(nDelayedLikeSingles);
  BR(promptLikeHz); BR(delayedLikeHz); BR(dmcEffSingles);
}
