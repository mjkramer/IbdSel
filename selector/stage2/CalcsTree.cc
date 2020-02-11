#include "CalcsTree.hh"

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
