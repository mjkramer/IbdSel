#include "Readers.hh"

MuonReader::MuonReader() :
  TimeSyncReader({"muons"})
{
  setClockMode(ClockMode::ClockReader);
  setLeadtime_us(4'500'000);
}

// -----------------------------------------------------------------------------

AdReader::AdReader(Det det, bool clockWriter) :
  TimeSyncReader({Form("physics_AD%d", int(det))},
                 clockWriter),
  det(det) {}

