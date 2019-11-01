#pragma once

#include "SelectorFramework/core/TimeSyncReader.cc"

#include "MuonTree.cc"
#include "ClusterTree.cc"

class MuonReader : public TimeSyncReader<MuonTree> {
public:
  MuonReader() :
    TimeSyncReader({"muons"})
  {
    setClockMode(ClockMode::ClockReader);
    setEpsilon_us(1'000'000);
    setPrefetch_us(1'000'000);
  }

  Time timeInTree() override
  {
    return data.time();
  }
};

// Don't forget to put a PrefetchHelper<MuonReader> somewhere between MuonReader
// and ClusterReader in the pipeline!

class ClusterReader : public TimeSyncReader<ClusterTree> {
public:
  ClusterReader(int detector) :
    TimeSyncReader({Form("clusters_AD%d", detector)}),
    detector(detector)
  {
    setClockMode(ClockMode::ClockReader);
    setEpsilon_us(1000);
  }

  const int detector;

  Time timeInTree() override
  {
    return data.time(data.size - 1);
  }
};

class SingleReader : public TimeSyncReader<ClusterTree> {
  SingleReader(int detector) :
    TimeSyncReader({Form("singles_AD%d", detector)}),
    detector(detector)
  {
    setClockMode(ClockMode::ClockWriter);
  }

  const int detector;

  Time timeInTree() override
  {
    return data.time(data.size - 1);
  }
};
