#include "EventReader.hh"

#include <algorithm>
#include <fstream>
#include <iterator>

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

  BR(hitChannels);
}

void EventReader::initWpMasks(const char* wpMaskFile)
{
  if (wpMaskFile && *wpMaskFile != 0) {
    std::ifstream f(wpMaskFile);
    std::copy(std::istream_iterator<int>(f), std::istream_iterator<int>(),
              std::inserter(maskedWpChans, maskedWpChans.end()));
  }
}

void EventReader::postReadCallback()
{
  if ((data.detector == 5 || data.detector == 6)
      && not maskedWpChans.empty()) {
    for (int chan : *data.hitChannels) {
      if (maskedWpChans.count(chan))
        --data.nHit;
    }
  }
}
