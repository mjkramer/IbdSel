#pragma once

// TODO: Also save singles here

#include "SelectorFramework/core/RingBuf.cc"
#include "SelectorFramework/core/Util.cc"

#include "EventReader.cc"

using Status = Algorithm::Status;

class ClusterAlg : public SimpleAlg<EventReader> {
  static constexpr float GAPSIZE_US = 1000;

public:
  ClusterAlg(int detector) : detector(detector) {}

  Status consume(const EventReader::Data& e) override;

  bool ready() const { return foundCluster; };

  const std::vector<EventReader::Data>& getCluster() const
  {
    return events;
  }

  const short detector;

private:
  std::vector<EventReader::Data> events;
  EventReader::Data pendingEvent;
  bool foundCluster = false;
};

Status ClusterAlg::consume(const EventReader::Data& e)
{
  if (foundCluster) {                 // Did we find a cluster on prev exec?
    foundCluster = false;
    events.clear();
    events.push_back(pendingEvent);
  }

  if (e.detector != detector)
    return Status::Continue;

  if (e.energy < 0.7)
    return Status::Continue;

  if (events.size() == 0) {     // Very first event
    events.push_back(e);
    return Status::Continue;
  }

  const Time t_this = e.time();
  const Time t_last = events.back().time();

  if (t_this.diff_us(t_last) > GAPSIZE_US) {
    if (events.size() == 1) {   // Just a single?
      events.clear();
      events.push_back(e);
    } else {                    // Found a cluster
      pendingEvent = e;
      foundCluster= true;
    }
  } else {
    events.push_back(e);
  }

  return Status::Continue;
}
