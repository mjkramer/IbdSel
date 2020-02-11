#include "MuonAlg.hh"

#include "Selectors.hh"

#include "../common/Constants.hh"

#include "../SelectorFramework/core/Util.hh"

#include <algorithm>
#include <iostream>

void MuonAlg::initCuts(const Config* config)
{
  BEGIN_CONFIG(config);

  CONFIG(muPreVeto_us);

  CONFIG(wpMuNhitCut);
  CONFIG(wpMuPostVeto_us);

  CONFIG(adMuChgCut);
  CONFIG(adMuPostVeto_us);

  CONFIG(showerMuChgCut);
  CONFIG(showerMuPostVeto_us);

  END_CONFIG();
}

void MuonAlg::connect(Pipeline& p)
{
  if (purpose == Purpose::ForIBDs) {
    auto config = p.getTool<Config>();
    initCuts(config);
  } // Otherwise just use default cuts

  SimpleAlg::connect(p);
}

double MuonAlg::vetoTime_s(Det detector) const
{
  return vetoTime_s_[int(detector) - 1];
}

inline Time MuonAlg::endOfLastVeto(size_t idet) const
{
  const Time wpEnd = lastWpTime.shifted_us(wpMuPostVeto_us);
  const Time adEnd = lastAdTime[idet].shifted_us(adMuPostVeto_us);
  const Time showerEnd = lastShowerTime[idet].shifted_us(showerMuPostVeto_us);

  return std::max({wpEnd, adEnd, showerEnd});
}

inline bool MuonAlg::isWP(const MuonTree& e) const
{
  return e.inWP() && e.strength > wpMuNhitCut;
}

inline bool MuonAlg::isShower(const MuonTree& e) const
{
  return e.inAD() && e.strength > showerMuChgCut;
}

inline bool MuonAlg::isAD(const MuonTree& e) const
{
  return e.inAD() && e.strength > adMuChgCut && !isShower(e);
}

inline float MuonAlg::nomPostVeto_us(const MuonTree& e) const
{
  if (isWP(e))
    return wpMuPostVeto_us;
  else if (isShower(e))
    return showerMuPostVeto_us;
  else if (isAD(e))
    return adMuPostVeto_us;
  else
    throw std::runtime_error("What kind of muon is this?");
}

float MuonAlg::effVeto_us(const MuonTree& e, size_t idet) const
{
  const Time lastEnd = endOfLastVeto(idet);
  const double delta_us = e.time().diff_us(lastEnd);
  const double post_us = nomPostVeto_us(e);

  if (delta_us >= muPreVeto_us)
    return muPreVeto_us + post_us;

  if (0 <= delta_us && delta_us < muPreVeto_us)
    return delta_us + post_us;

  if (-post_us <= delta_us && delta_us < 0)
    return delta_us + post_us;

  else                          // delta_us < -post_us
    return 0;
}

Algorithm::Status MuonAlg::consume(const MuonTree& e)
{
  if (!isWP(e) && !isAD(e) && !isShower(e))
    return Status::Continue;

  if (isWP(e)) {
    for (size_t idet = 0; idet < 4; ++idet)
      vetoTime_s_[idet] += 1e-6 * effVeto_us(e, idet);
    lastWpTime = e.time();
  }

  else if (isAD(e) || isShower(e)) {
    const size_t idet = idx_of(e.detector);
    vetoTime_s_[idet] += 1e-6 * effVeto_us(e, idet);
    if (isShower(e))
      lastShowerTime[idet] = e.time();
    else
      lastAdTime[idet] = e.time();
  }

  muonBuf.put(e);

  return Status::Continue;
}

bool MuonAlg::isVetoed(Time t, Det detector) const
{
  bool xfirst = true;
  const char* xpurp = (purpose == Purpose::ForIBDs) ? "IBDs" : "singles";
  const char* xheader = Form("AD%d %s: ", int(detector), xpurp);

  const size_t idet = size_t(detector) - size_t(Det::AD1);
  size_t i = lastBufDepth[idet];

  for (; i > 0; --i) {          // going forward in time
    const auto& muon = muonBuf.at(i);
    const float dt_us = t.diff_us(muon.time());
    if (dt_us < -muPreVeto_us)
      break;
  }

  for (; i < muonBuf.size(); ++i) { // going back in time
    const auto& muon = muonBuf.at(i);
    const float dt_us = t.diff_us(muon.time());

    if (xfirst) {
      xfirst = false;
      if (dt_us > -muPreVeto_us) {
        std::cout << xheader << Form("MuonAlg is behind! %.3f", dt_us) << std::endl;
      }
    }


    if (showerMuPostVeto_us < dt_us) {  // no more muons worth checking
      return false;
    }

    if (dt_us < -muPreVeto_us)  // way-after-event muon
      continue;

    if (muon.inAD() && muon.detector != detector)
      continue;                 // Ignore muons in other ADs

    if (-muPreVeto_us < dt_us && dt_us < 0) { // pre-muon veto
      lastBufDepth[idet] = i;
      return true;
    }

    if (dt_us < nomPostVeto_us(muon)) {
      lastBufDepth[idet] = i;
      return true;
    }
  }

  if (muonBuf.full()) {
    std::cout << xheader << "MuonAlg is exhausted!" << std::endl;
  }

  return false;
}
