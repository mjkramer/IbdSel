#include "Readers.hh"

MuonReader::MuonReader() :
  TimeSyncReader({"muons"})
{
  setClockMode(ClockMode::ClockReader);
  setEpsilon_us(3'000'000);
  setPrefetch_us(4'500'000);
}

// -----------------------------------------------------------------------------

AdReader::AdReader(Det det, bool clockWriter) :
  TimeSyncReader({Form("physics_AD%d", int(det))},
                 clockWriter),
  det(det) {}

