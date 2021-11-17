#include "NonUniCorr.hh"
#include "Readers.hh"
#include "stage2_globals.hh"

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


void AdReader::connect(Pipeline& p)
{
  TimeSyncReader<AdTree>::connect(p);

  try {
    nonUniCorr = p.getTool<NonUniCorrTool>();
  } catch (...) {
    // If we land here, NonUniCorrTool was never loaded, i.e. the correction is
    // not enabled.
    nonUniCorr = nullptr;
  }
}

void AdReader::postReadCallback()
{
  if (nonUniCorr)
    data.energy = nonUniCorr->correctedEnergy(det, data);
}
