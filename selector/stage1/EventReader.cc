#pragma once

#include "../common/Constants.cc"

#include "../SelectorFramework/core/SyncReader.cc"
#include "../SelectorFramework/core/Util.cc"

class EventTree : public TreeBase {
public:
  EventTree() {}
  EventTree(bool useSCNL) : useSCNL(useSCNL) {}

  // RecHeader (AdSimple etc.)
  Short_t detector;
  UInt_t triggerType, triggerNumber, triggerTimeSec, triggerTimeNanoSec;
  Float_t energy;

  // CalibStats
  Int_t nHit;
  Float_t Quadrant, MaxQ, MaxQ_2inchPMT, time_PSD, time_PSD1;
  Float_t NominalCharge;
  Float_t integralRunTime_ms;

  Det det() const { return Det(detector); }
  Time time() const { return { triggerTimeSec, triggerTimeNanoSec }; };
  bool isAD() const { return detector <= 4; }
  bool isWP() const { return detector == 5 || detector == 6; }

  void initBranches() override;

private:
  bool useSCNL;
};

void EventTree::initBranches()
{
  BR(detector);
  BR(triggerType); BR(triggerNumber); BR(triggerTimeSec); BR(triggerTimeNanoSec);
  BR(energy);

  BR(nHit);
  BR(Quadrant); BR(MaxQ); BR(MaxQ_2inchPMT); BR(time_PSD); BR(time_PSD1);
  BR(integralRunTime_ms);

  BR_NAMED(NominalCharge, useSCNL ? "NominalChargeNL" : "NominalCharge");
}

class EventReader : public SyncReader<EventTree> {
public:
  EventReader(bool useSCNL=false) :
    SyncReader({useSCNL ? "/Event/Rec/AdSimpleNL" : "/Event/Rec/AdSimple",
                "/Event/Data/CalibStats"},
      useSCNL /* forwarded to EventTree constructor */)
  {}
};
