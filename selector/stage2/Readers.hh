#pragma once

#include "../common/AdTree.hh"
#include "../common/Constants.hh"
#include "../common/MuonTree.hh"

#include "../SelectorFramework/core/TimeSyncReader.hh"

class NonUniCorrTool;

class MuonReader : public TimeSyncReader<MuonTree> {
public:
  MuonReader();

  Time timeInTree() override;
};

class AdReader : public TimeSyncReader<AdTree> {
public:
  AdReader(Det det, ClockMode clockMode);
  void connect(Pipeline& p) override;

  int rawTag() const override;
  Time timeInTree() override;

  const Det det;

private:
  NonUniCorrTool* nonUniCorr;
  void postReadCallback() override;
};

inline
Time MuonReader::timeInTree()
{
  return data.time();
}

inline
int AdReader::rawTag() const
{
  return int(det);
}

inline
Time AdReader::timeInTree()
{
  return data.time();
}
