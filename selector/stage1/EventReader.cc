#include "EventReader.hh"

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
