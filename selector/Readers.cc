#pragma once

#include "SelectorFramework/core/TimeSyncReader.cc"

#include "Constants.cc"
#include "MuonTree.cc"
#include "AdTree.cc"
#include "ClusterTree.cc"

class MuonReader : public TimeSyncReader<MuonTree> {
public:
  MuonReader() :
    TimeSyncReader({"muons"})
  {
    setClockMode(ClockMode::ClockReader);
    setEpsilon_us(3'000'000);
    setPrefetch_us(4'500'000);
  }

  Time timeInTree() override
  {
    return data.time();
  }
};

class AdReader : public TimeSyncReader<AdTree> {
public:
  AdReader(Det det, bool clockWriter = false) :
    TimeSyncReader({Form("physics_AD%d", int(det))},
                   clockWriter),
    det(det) {}

  int getTag() const override { return int(det); }

  Time timeInTree() override { return data.time(); }

  const Det det;
};

// Don't forget to put a PrefetchHelper<MuonReader> somewhere between MuonReader
// and ClusterReader in the pipeline!

class BaseClusterReader : public TimeSyncReader<ClusterTree> {
public:
  BaseClusterReader(Det detector,
                    const char* treeNameTemplate,
                    bool clockWriter = false) :
    detector(detector),
    TimeSyncReader({Form(treeNameTemplate, int(detector))},
                   clockWriter) {}

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
  ClusterReader(Det detector, bool clockWriter = false) :
    BaseClusterReader(detector, "clusters_AD%d", clockWriter)
  {
    setEpsilon_us(1000);
    setPrefetch_us(2000);
  }
};

class SingleReader : public BaseClusterReader {
public:
  SingleReader(Det detector, bool clockWriter = false) :
    BaseClusterReader(detector, "singles_AD%d", clockWriter)
  {
    setEpsilon_us(5000);
    setPrefetch_us(10000);
  }
};
