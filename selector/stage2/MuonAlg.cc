#include "MuonAlg.hh"

#include "Selectors.hh"

#include "../common/Constants.hh"

#include "../SelectorFramework/core/Util.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <cctype>               // toupper

template <typename T>
static T readconf(const Config* config, const char* prefix, std::string name)
{
  name[0] = toupper(name[0]);
  name = std::string(prefix) + name;
  return config->get<T>(name.c_str());
}

void MuonAlg::initCuts(const Config* config)
{
  const char* prefix = purpose == Purpose::ForIBDs ? "ibd" : "single";

#define F(var) var = readconf<float>(config, prefix, #var)
#define I(var) var = readconf<int>(config, prefix, #var)

  I(muPreVeto_us);

  F(wpMuNhitCut);
  F(wpMuPostVeto_us);

  F(adMuChgCut);
  F(adMuPostVeto_us);

  F(showerMuChgCut);
  F(showerMuPostVeto_us);

#undef F
#undef I

  extraIwsVeto = config->get<bool>("extraIwsVeto");
  extraIwsNhitAtLeast = config->get<int>("extraIwsNhitAtLeast");
  extraIwsPostVeto_us = config->get<float>("extraIwsPostVeto_us");
}

void MuonAlg::log(const char* msg, Det det, Time t, const MuonTree* muon) const
{
  const bool forIBDs = purpose == Purpose::ForIBDs;
  const char* selName = forIBDs ? "IBDs" : "singles";

  const auto sel = forIBDs
    ? static_cast<SelectorBase*>(pipe().getAlg<IbdSel>(det))
    : static_cast<SelectorBase*>(pipe().getAlg<SingleSel>(det));

  const AdTree& delayed = sel->getCurrent();

  const char* maybeMuonTime =
    muon ? TmpStr(", T_mu %d.%d", muon->time().s, muon->time().ns)
         : "";

  std::cerr << TmpStr("MuonAlg (%s) %d %d, AD%d trig %d%s: %s\n", selName,
                      delayed.runNo, delayed.fileNo, det, delayed.trigNo,
                      maybeMuonTime, msg);
}

void MuonAlg::connect(Pipeline& p)
{
  auto config = p.getTool<Config>();
  initCuts(config);

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
  const Time extraIwsEnd = lastExtraIwsTime.shifted_us(extraIwsPostVeto_us);

  return std::max({wpEnd, adEnd, showerEnd, extraIwsEnd});
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

inline bool MuonAlg::isExtraIws(const MuonTree& e) const
{
  return e.detector == Det::IWS
    && e.strength >= extraIwsNhitAtLeast && e.strength <= wpMuNhitCut;
}

inline float MuonAlg::nomPostVeto_us(const MuonTree& e) const
{
  if (isWP(e))
    return wpMuPostVeto_us;
  else if (isShower(e))
    return showerMuPostVeto_us;
  else if (isAD(e))
    return adMuPostVeto_us;
  else if (isExtraIws(e))
    return extraIwsPostVeto_us;
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

  else if (isExtraIws(e)) {
    for (size_t idet = 0; idet < 4; ++idet)
      vetoTime_s_[idet] += 1e-6 * effVeto_us(e, idet);
    lastExtraIwsTime = e.time();
  }

  muonBuf.put(e);

  return Status::Continue;
}

bool MuonAlg::isVetoed(Time t, Det detector) const
{
  bool first = true;

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

    if (first) {
      first = false;
      if (dt_us > -muPreVeto_us)
        log(TmpStr("MuonAlg is behind! %.3f", dt_us),
            detector, t, &muon);
    }


    if (showerMuPostVeto_us < dt_us)  // no more muons worth checking
      return false;

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

  if (muonBuf.full())
    log("MuonAlg is exhausted!", detector, t);

  return false;
}
