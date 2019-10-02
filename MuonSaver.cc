#pragma once

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
  bool isMuon() const { return isMuon_; }

private:
  MuonTree outTree;
  bool isMuon_;
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
  isMuon_ = false;

  auto put = [&](Float_t strength) {
    isMuon_ = true;

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
