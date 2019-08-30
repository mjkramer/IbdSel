#pragma once

#include "SelectorFramework/core/RingBuf.cc"
#include "SelectorFramework/core/Util.cc"

#include "EventReader.cc"

using Status = Algorithm::Status;

class MuonAlg : public SimpleAlg<EventReader> {
  static constexpr float WP_NHIT_CUT = 12;
  static constexpr float AD_CHG_CUT = 3000;
  static constexpr float SHOWER_CHG_CUT = 300'000;
  static constexpr unsigned N_MUONS = 1000; // how far back to remember

public:
  enum class Kind { WP, AD, Shower };

  struct Muon {
    short detector;
    Kind kind;
    Time t;
  };

  MuonAlg() : muonBuf(N_MUONS) {}

  Status consume(const EventReader::Data& e) override;

  const RingBuf<Muon>& getBuf() const { return muonBuf; }

private:
  RingBuf<Muon> muonBuf;

};

Status MuonAlg::consume(const EventReader::Data& e)
{
  auto put = [&](auto kind) {
    muonBuf.put({e.detector, kind, e.time()});
  };

  if (e.detector == 5 || e.detector == 6) {
    if (e.nHit > WP_NHIT_CUT)
      put(Kind::WP);
  }

  else if (e.detector < 5) {
    if (e.NominalCharge > SHOWER_CHG_CUT)
      put(Kind::Shower);
    else if (e.NominalCharge > AD_CHG_CUT)
      put(Kind::AD);
  }

  return Status::Continue;
}
