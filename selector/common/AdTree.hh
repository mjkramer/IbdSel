#pragma once

#include "../SelectorFramework/core/BaseIO.hh"
#include "../SelectorFramework/core/Util.hh"

class AdTree : public TreeBase {
public:
  UInt_t trigSec;
  UInt_t trigNanoSec;
  Float_t energy;
  Float_t x, y, z;

  UInt_t trigNo;
  UInt_t runNo;
  UShort_t fileNo;

  Time time() const { return { trigSec, trigNanoSec }; }

  void initBranches() override;
};
