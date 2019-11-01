#pragma once

#include "SelectorFramework/core/BaseIO.cc"

class MuonTree : public TreeBase {
public:
  UChar_t detector;
  UInt_t trigSec;
  UInt_t trigNanoSec;
  Float_t strength;             // charge (AD) or nhit (WP)

  Time time() const { return {trigSec, trigNanoSec}; }

  void initBranches() override;
};

void MuonTree::initBranches()
{
  BR(detector);
  BR(trigSec);
  BR(trigNanoSec);
  BR(strength);
}
