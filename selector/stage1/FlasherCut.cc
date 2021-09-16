#include "FlasherCut.hh"

Algorithm::Status flasherCut(const EventReader::Data& e)
{
#define SQ(x) pow(x, 2)
  bool flasher = SQ(e.Quadrant) + SQ(e.MaxQ / 0.45) > 1
    || 4*SQ(1 - e.time_PSD) + 1.8*SQ(1 - e.time_PSD1) > 1
    || e.MaxQ_2inchPMT > 100;

  bool topFlasher = e.time_PSD_local_RMS > 13 && e.Kurtosis > 1
    && e.flasher_ring == 8
    && e.flasher_column >= 4 && e.flasher_column <= 9;

  return vetoIf(flasher || topFlasher);
#undef SQ
}

