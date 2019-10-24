#pragma once

#include "BaseIO.hh"

class MuonTree : public TreeBase {
public:
  UChar_t detector;
  UInt_t trigSec;
  UInt_t trigNanoSec;
  Float_t strength;             // charge (AD) or nhit (WP)

  void initBranches() override;
};

inline void MuonTree::initBranches()
{
  BR(detector);
  BR(trigSec);
  BR(trigNanoSec);
  BR(strength);
}
