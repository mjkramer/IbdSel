#pragma once

#include "EventReader.hh"

#include "../common/Constants.hh"

#include "../SelectorFramework/core/Kernel.hh"
#include "../SelectorFramework/core/SimpleAlg.hh"

#include <TH1F.h>

using Status = Algorithm::Status;

class LivetimeSaver : public SimpleAlg<EventReader> {
public:
  Status consume(const EventReader::Data& data) override;
  void finalize(Pipeline& pipeline) override;

private:
  float last_integralRunTime_ms;
};
