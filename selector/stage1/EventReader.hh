#pragma once

#include "../common/Constants.hh"

#include "../SelectorFramework/core/SyncReader.hh"
#include "../SelectorFramework/core/Util.hh"

class EventTree : public TreeBase {
public:
  EventTree() {}
  EventTree(bool useSCNL) : useSCNL(useSCNL) {}

  // RecHeader (AdSimple etc.)
  Short_t detector;
  UInt_t triggerType, triggerNumber, triggerTimeSec, triggerTimeNanoSec;
  Float_t energy;
  Float_t x, y, z;

  // CalibStats
  Int_t nHit;
  Float_t Quadrant, MaxQ, MaxQ_2inchPMT, time_PSD, time_PSD1;
  Float_t NominalCharge;
  Float_t integralRunTime_ms;

  Det det() const;
  Time time() const;
  bool isAD() const;
  bool isWP() const;

  void initBranches() override;

private:
  bool useSCNL;
};

inline Det EventTree::det() const
{
  return Det(detector);
}

inline Time EventTree::time() const
{
  return {triggerTimeSec, triggerTimeNanoSec};
}

inline bool EventTree::isAD() const
{
  return detector <= 4;
}

inline bool EventTree::isWP() const
{
  return detector == 5 || detector == 6;
}

class EventReader : public SyncReader<EventTree> {
public:
  EventReader(bool useSCNL=false) :
    SyncReader({useSCNL ? "/Event/Rec/AdSimpleNL" : "/Event/Rec/AdSimple",
        "/Event/Data/CalibStats"},
      useSCNL /* forwarded to EventTree constructor */)
  {}
};
