#pragma once

#include "RingBuf.hh"
#include "Util.hh"

#include "EventReader.hh"

using Status = Algorithm::Status;

class MuonAlg : public SimpleAlg<EventReader> {
  static constexpr unsigned N_MUONS = 1000; // how far back to remember
  static constexpr float WP_NHIT_CUT = 12;
  static constexpr float AD_CHG_CUT = 3000;
  static constexpr float SHOWER_CHG_CUT = 300000;
  static constexpr float WP_VETO = 600;
  static constexpr float AD_VETO = 1400;
  static constexpr float SHOWER_VETO = 400400;
  static constexpr float PRE_VETO = 2;

public:
  enum class Kind { WP, AD, Shower };

  struct Muon {
    short detector;
    Kind kind;
    Time t;
  };

  MuonAlg() : muonBuf(N_MUONS) {}

  Status consume(const EventReader::Data& e) override;

  bool isVetoed(const EventReader::Data& e) const;

  const RingBuf<Muon>& getBuf() const { return muonBuf; }

private:
  RingBuf<Muon> muonBuf;

};

inline Status MuonAlg::consume(const EventReader::Data& e)
{
  auto put = [&](Kind kind) {
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

inline bool MuonAlg::isVetoed(const EventReader::Data& event) const
{
  const Time t = event.time();

  for (const auto& muon : muonBuf) {
    const float dt_us = t.diff_us(muon.t);

    if (SHOWER_VETO < dt_us)            // no more muons worth checking
      break;

    if (dt_us < -PRE_VETO)              // way-after-event muon
      continue;

    if (muon.detector < 5 && muon.detector != event.detector)
      continue;                         // Ignore muons in other ADs

    if (-PRE_VETO < dt_us && dt_us < 0) // pre-muon veto
      return true;

    const auto postMuonVeto_us = [&]() -> float {
      switch (muon.kind) {
      case MuonAlg::Kind::WP:     return WP_VETO;
      case MuonAlg::Kind::AD:     return AD_VETO;
      case MuonAlg::Kind::Shower: return SHOWER_VETO;
      } return {};              // silence compiler warning
    }();

    if (dt_us < postMuonVeto_us)
      return true;
  }

  return false;
}
