#pragma once

#include "AdTree.hh"
#include "Constants.hh"
#include "MuonTree.hh"

#include "TimeSyncReader.hh"

class MuonReader : public TimeSyncReader<MuonTree> {
public:
  MuonReader();

  Time timeInTree() override;
};

class AdReader : public TimeSyncReader<AdTree> {
public:
  AdReader(Det det, ClockMode clockMode);

  int rawTag() const override;
  Time timeInTree() override;

  const Det det;
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
