#pragma once

#include "EventReader.hh"

#include "Constants.hh"

#include "Kernel.hh"
#include "SimpleAlg.hh"

#include <TH1F.h>

using Status = Algorithm::Status;

class LivetimeSaver : public SimpleAlg<EventReader> {
public:
  Status consume(const EventReader::Data& data) override;
  void finalize(Pipeline& pipeline) override;

private:
  float last_integralRunTime_ms;
};
