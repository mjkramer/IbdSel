#pragma once

#include "../common/AdTree.cc"
#include "../common/Constants.cc"
#include "../common/MuonTree.cc"

#include "../SelectorFramework/core/TimeSyncReader.cc"

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

  int rawTag() const override { return int(det); }

  Time timeInTree() override { return data.time(); }

  const Det det;
};
