#pragma once

#include "../SelectorFramework/core/BaseIO.cc"

class IbdTree : public TreeBase {
public:
  UInt_t runNo;
  UInt_t fileNo;
  UInt_t trigP;
  UInt_t trigD;

  void initBranches() override;
};

void IbdTree::initBranches()
{
  BR(runNo);
  BR(fileNo);
  BR(trigP);
  BR(trigD);
}
