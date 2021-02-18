#pragma once

#include "../SelectorFramework/core/BaseIO.hh"

class IbdTree : public TreeBase {
public:
  UInt_t runNo;
  UInt_t fileNo;
  UInt_t trigP;
  UInt_t trigD;
  Float_t eP;
  Float_t eD;
  Float_t dt_us;
  Float_t xP, yP, zP;
  Float_t xD, yD, zD;

  void initBranches() override;
};
