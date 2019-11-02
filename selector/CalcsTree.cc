#pragma once

#include "Constants.cc"

#include "SelectorFramework/core/BaseIO.cc"

class CalcsTree : public TreeBase {
public:
  Float_t livetime_s;
  Float_t vetoEff;
  Float_t dmcEff;

  Float_t accDaily, accDailyErr;
  Float_t li9Daily, li9DailyErr;

  UInt_t seq;
  Stage stage;
  Site site;
  Det detector;

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
  BR(stage);
  BR(site);
  BR(detector);
}
