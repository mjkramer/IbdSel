#include "EventReader.hh"

void EventTree::initBranches()
{
  BR(detector);
  BR(triggerType); BR(triggerNumber); BR(triggerTimeSec); BR(triggerTimeNanoSec);
  BR(energy);
  BR(x); BR(y); BR(z);

  BR(nHit);
  BR(Quadrant); BR(MaxQ); BR(MaxQ_2inchPMT); BR(time_PSD); BR(time_PSD1);
  BR(time_PSD_local_RMS); BR(Kurtosis);
  BR(flasher_ring); BR(flasher_column);

  BR(integralRunTime_ms);

  BR_NAMED(NominalCharge, useSCNL ? "NominalChargeNL" : "NominalCharge");
}
