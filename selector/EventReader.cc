#pragma once

#include "SelectorFramework/core/SeqReader.cc"
#include "SelectorFramework/core/Util.cc"

class EventReader : public SeqReader {
public:
  EventReader(bool useSCNL=false) :
    SeqReader{useSCNL ? "/Event/Rec/AdSimpleNL" : "/Event/Rec/AdSimple",
              "/Event/Data/CalibStats"},
    useSCNL(useSCNL) {}

  struct Data {
    // RecHeader (AdSimple etc.)
    Short_t detector;
    UInt_t triggerType, triggerNumber, triggerTimeSec, triggerTimeNanoSec;
    Float_t energy;

    // CalibStats
    Int_t nHit;
    Float_t Quadrant, MaxQ, MaxQ_2inchPMT, time_PSD, time_PSD1;
    Float_t NominalCharge;

    Time time() const { return { triggerTimeSec, triggerTimeNanoSec }; };
    bool isAD() const { return detector <= 4; }
    bool isWP() const { return detector == 5 || detector == 6; }
  } data;

private:
  void initBranches() override;

  bool useSCNL;
};

inline void EventReader::initBranches()
{
  BR(detector);
  BR(triggerType); BR(triggerNumber); BR(triggerTimeSec); BR(triggerTimeNanoSec);
  BR(energy);

  BR(nHit);
  BR(Quadrant); BR(MaxQ); BR(MaxQ_2inchPMT); BR(time_PSD); BR(time_PSD1);

  initBranch(useSCNL ? "NominalChargeNL" : "NominalCharge", &data.NominalCharge);
}
