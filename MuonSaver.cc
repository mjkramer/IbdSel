#pragma once

#include "SelectorFramework/core/RingBuf.cc"
#include "SelectorFramework/core/Util.cc"
#include "SelectorFramework/core/OutTree.cc"

#include "EventReader.cc"

using Status = Algorithm::Status;

class MuonTree : public OutTree {
public:
  using OutTree::OutTree;

  UChar_t detector;
  UInt_t trigSec;
  UInt_t trigNanoSec;
  Float_t strength;

private:
  void initBranches() override;
};

void MuonTree::initBranches()
{
  OB(detector, "b");
  OB(trigSec, "i");
  OB(trigNanoSec, "i");
  OB(strength, "F");
}

class MuonSaver : public SimpleAlg<EventReader> {
  static constexpr int WP_MIN_NHIT = 12;
  static constexpr float AD_MIN_CHG = 3000;

public:
  MuonSaver();
  void connect(Pipeline& pipeline) override;
  Status consume(const EventReader::Data& e) override;

private:
  MuonTree outTree;
};

MuonSaver::MuonSaver() :
  outTree("muons")
{
}

void MuonSaver::connect(Pipeline& pipeline)
{
  outTree.connect(pipeline);
  SimpleAlg::connect(pipeline);
}

Status MuonSaver::consume(const EventReader::Data& e)
{
  auto put = [&](Float_t strength) {
    outTree.detector = e.detector;
    outTree.trigSec = e.time().s;
    outTree.trigNanoSec = e.time().ns;
    outTree.strength = strength;

    outTree.fill();
  };

  if (e.isAD() && e.NominalCharge > AD_MIN_CHG)
    put(e.NominalCharge);

  else if (e.isWP() && e.nHit > WP_MIN_NHIT)
    put(e.nHit);

  return Status::Continue;
}
