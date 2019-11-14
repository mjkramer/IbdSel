#pragma once

#include "SelectorFramework/core/TimeSyncReader.cc"

#include "Constants.cc"
#include "MuonTree.cc"
#include "ClusterTree.cc"

class MuonReader : public TimeSyncReader<MuonTree> {
public:
  MuonReader() :
    TimeSyncReader({"muons"})
  {
    setClockMode(ClockMode::ClockReader);
    setEpsilon_us(500'000);
    setPrefetch_us(1'000'000);
  }

  Time timeInTree() override
  {
    return data.time();
  }
};

// Don't forget to put a PrefetchHelper<MuonReader> somewhere between MuonReader
// and ClusterReader in the pipeline!

class BaseClusterReader : public TimeSyncReader<ClusterTree> {
public:
  BaseClusterReader(Det detector, const char* treeNameTemplate) :
    detector(detector),
    TimeSyncReader({Form(treeNameTemplate, int(detector))}) {}

  int getTag() const override
  {
    return int(detector);
  }

  Time timeInTree() override
  {
    return data.time(data.size - 1);
  }

  const Det detector;
};

class ClusterReader : public BaseClusterReader {
public:
  ClusterReader(Det detector) :
    BaseClusterReader(detector, "clusters_AD%d")
  {
    setClockMode(ClockMode::ClockReader);
    setEpsilon_us(10000);
    setPrefetch_us(20000);
  }
};

class SingleReader : public BaseClusterReader {
public:
  SingleReader(Det detector, bool clockWriter) :
    BaseClusterReader(detector, "singles_AD%d")
  {
    if (clockWriter) {
      setClockMode(ClockMode::ClockWriter);
    } else {
      setClockMode(ClockMode::ClockReader);
      setEpsilon_us(10000);
      setPrefetch_us(20000);
    }
  }
};
