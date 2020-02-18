#include "Readers.hh"

MuonReader::MuonReader() :
  TimeSyncReader({"muons"}, ClockMode::ClockReader)
{
  leadtime_us = 4'500'000;
}

// -----------------------------------------------------------------------------

AdReader::AdReader(Det det, ClockMode clockMode) :
  TimeSyncReader({Form("physics_AD%d", int(det))},
                 clockMode),
  det(det) {}

