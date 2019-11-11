#pragma once

#include "Readers.cc"
#include "Constants.cc"

#include "SelectorFramework/core/ConfigTool.cc"
#include "SelectorFramework/core/RingBuf.cc"
#include "SelectorFramework/core/Util.cc"

#include <algorithm>

class MuonAlg : public SimpleAlg<MuonReader> {
  static constexpr unsigned BUF_SIZE = 1000;

public:
  enum class Purpose { ForIBDs, ForSingles };

  MuonAlg(Purpose purp) :
    purpose(purp), muonBuf(BUF_SIZE) {}

  int getTag() const override { return int(purpose); }

  void connect(Pipeline& p) override;
  Algorithm::Status consume(const MuonTree& e) override;

  bool isVetoed(Time t, Det detector) const;
  double vetoTime_s(Det detector) const;

private:
  void initCuts(const Config* config);
  Time endOfLastVeto(size_t idet) const;
  bool isWP(const MuonTree& e) const;
  bool isShower(const MuonTree& e) const;
  bool isAD(const MuonTree& e) const;
  float nomPostVeto_us(const MuonTree& e) const;
  float effVeto_us(const MuonTree& e, size_t idet) const;

  Purpose purpose;

  double muPreVeto_us = 2;

  int wpMuNhitCut = 12;
  double wpMuPostVeto_us = 600;

  double adMuChgCut = 3000;
  double adMuPostVeto_us = 1400;

  double showerMuChgCut = 300'000 ;
  double showerMuPostVeto_us = 400'400;

  Time lastWpTime;
  Time lastAdTime[4];
  Time lastShowerTime[4];

  RingBuf<MuonTree> muonBuf;
  double vetoTime_s_[4] = {0};
};

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
  } // Otherwise just use defaults

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
    lastWpTime = e.time();
    for (size_t idet = 0; idet < 4; ++idet)
      vetoTime_s_[idet] += 1e-6 * effVeto_us(e, idet);
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

struct maybe_cout {
  maybe_cout(bool on) : on_(on) {}

  template <class T>
  maybe_cout& operator<<(const T& t)
  {
    if (on_)
      std::cout << t;
    return *this;
  }

  // for endl: https://stackoverflow.com/questions/1134388/stdendl-is-of-unknown-type-when-overloading-operator

  template <class T> using Manip = T& (*)(T&);

  maybe_cout& operator<<(Manip<std::ostream> m)
  {
    return operator<< <Manip<std::ostream>> (m);
  }

  maybe_cout& operator<<(Manip<std::basic_ios<char>> m)
  {
    return operator<< <Manip<std::basic_ios<char>>> (m);
  }

  maybe_cout& operator<<(Manip<std::ios_base> m)
  {
    return operator<< <Manip<std::ios_base>> (m);
  }

  bool on_;
};

// template <class T>
// maybe_cout& operator<<(maybe_cout& mc, const T& t)
// {
//   if (mc.on_)
//     std::cout << t;
//   return mc;
// }

bool MuonAlg::isVetoed(Time t, Det detector) const
{
  bool xfirst = true;
  size_t xx = 0;
  maybe_cout xcout(purpose == Purpose::ForIBDs);

  for (const auto& muon : muonBuf) {
    const float dt_us = t.diff_us(muon.time());

    if (xfirst) {
      xcout << "first dt " << dt_us << std::endl;
      xfirst = false;
    }

    ++xx;

    if (showerMuPostVeto_us < dt_us) {  // no more muons worth checking
      xcout << "end at muon " << xx << std::endl;
      return false;
    }

    if (dt_us < -muPreVeto_us)  // way-after-event muon
      continue;

    if (muon.inAD() && muon.detector != detector)
      continue;                 // Ignore muons in other ADs

    if (-muPreVeto_us < dt_us && dt_us < 0) { // pre-muon veto
      xcout << "veto before muon " << xx << std::endl;
      return true;
    }

    if (dt_us < nomPostVeto_us(muon)) {
      xcout << "veto after muon " << xx << std::endl;
      return true;
    }
  }

  xcout << "exhausted after muon " << xx << std::endl;

  return false;
}
