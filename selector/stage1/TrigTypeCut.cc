#pragma once

#include "EventReader.cc"

Algorithm::Status trigTypeCut(const EventReader::Data& e)
{
  std::initializer_list<unsigned int> badTrigTypes = {
    0x10000001,    // manual
    0x10000002,    // cross
    0x10000004,    // periodic
    0x10000008,    // pedestal
    0x10000010,    // calib
    0x10000020,    // random
  };

  for (const auto mask : badTrigTypes) {
    if ((e.triggerType & mask) == mask)
      return Algorithm::Status::SkipToNext;
  }

  return Algorithm::Status::Continue;
}

using TrigTypeCut = PureAlg<EventReader, trigTypeCut>;
