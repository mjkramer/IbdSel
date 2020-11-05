#pragma once

#include "BaseIO.hh"
#include "Util.hh"

class AdTree : public TreeBase {
public:
  UInt_t trigSec;
  UInt_t trigNanoSec;
  Float_t energy;

  UInt_t trigNo;
  UInt_t runNo;
  UShort_t fileNo;

  Time time() const { return { trigSec, trigNanoSec }; }

  void initBranches() override;
};
