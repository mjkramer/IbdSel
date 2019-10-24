#pragma once

#include "TreeWriter.hh"

#include "EventReader.hh"
#include "MuonTree.hh"

using Status = Algorithm::Status;

class MuonSaver : public SimpleAlg<EventReader> {
  static constexpr int WP_MIN_NHIT = 12;
  static constexpr float AD_MIN_CHG = 3000;

public:
  MuonSaver();
  void connect(Pipeline& pipeline) override;
  Status consume(const EventReader::Data& e) override;
  bool isMuon() const { return isMuon_; }

private:
  TreeWriter<MuonTree> outTree;
  bool isMuon_;
};

inline MuonSaver::MuonSaver() :
  outTree("muons")
{
}

inline void MuonSaver::connect(Pipeline& pipeline)
{
  outTree.connect(pipeline);
  SimpleAlg::connect(pipeline);
}

inline Status MuonSaver::consume(const EventReader::Data& e)
{
  isMuon_ = false;

  auto put = [&](Float_t strength) {
    isMuon_ = true;

    outTree.data.detector = e.detector;
    outTree.data.trigSec = e.time().s;
    outTree.data.trigNanoSec = e.time().ns;
    outTree.data.strength = strength;

    outTree.fill();
  };

  if (e.isAD() && e.NominalCharge > AD_MIN_CHG)
    put(e.NominalCharge);

  else if (e.isWP() && e.nHit > WP_MIN_NHIT)
    put(e.nHit);

  return Status::Continue;
}
