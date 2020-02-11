#pragma once

#include "../common/Constants.cc"

#include <fstream>
#include <functional>
#include <map>
#include <string>

class Li9Calc {
public:
  Li9Calc();

  // This return the rate (per AD per day) predicted in our raw spectrum
  // assuming perfect veto/dmc efficiencies
  double li9daily(Site site, unsigned shower_pe, double showerVeto_ms);

private:
  static constexpr unsigned MIN_PE = 5'000;
  static constexpr unsigned MAX_NTAG_PE = 160'000;
  static constexpr unsigned MIN_VETO_BDRY_PE = 250'000;
  static constexpr unsigned MAX_VETO_BDRY_PE = 350'000;
  static constexpr unsigned DELTA_VETO_BDRY_PE = 10'000;
  static constexpr unsigned MAX_PE = 9'999'999;
  static constexpr double LI9_SEL_SHOWER_VETO_MS = 400;
  static constexpr double LI9_LIVETIME_MS = 257;
  static constexpr double NTAG_EFF = 0.45;
  static constexpr const char* LI9_FILENAME = "matt_Li9_rates.txt";
  static constexpr const char* LIVETIME_FILENAME = "p17b_livetimes.txt";

  struct Data {
    double nomRate, statUnc, rateNoB12, rate15pctHe8, rateNoHe8;
  };

  struct MuonBin {
    unsigned lowerPE, upperPE;

    bool operator<(const MuonBin& rhs) const // needed for std::map
    {
      return lowerPE < rhs.lowerPE ||
        (lowerPE == rhs.lowerPE && upperPE < rhs.upperPE);
    }
  };

  Data& get(Site site, MuonBin bin)
  {
    return table[site][bin];
  }

  using SiteTable = std::map<MuonBin, Data>;
  using Table = std::map<Site, SiteTable>;
  using LivetimeTable = std::map<Site, double>;

  std::string dataPath(const char* filename);
  void initTable();
  void initLivetimes();
  double measLowRange(Site site);
  double measMidRange(Site site, unsigned shower_pe);
  double measHighRange(Site site, unsigned shower_pe);
  double extrapolate(unsigned shower_pe,
                     std::function<double(unsigned)> getter);

  Table table;
  LivetimeTable adLivedays;
};

Li9Calc::Li9Calc()
{
  initTable();
  initLivetimes();
}

void Li9Calc::initTable()
{
  std::ifstream ifs(dataPath(LI9_FILENAME));

  while (true) {
    unsigned site;
    ifs >> site;

    if (not ifs)      // eof
      break;

    MuonBin bin;
    ifs >> bin.lowerPE >> bin.upperPE;

    Data& d = table[Site(site)][bin];
    ifs >> d.nomRate >> d.statUnc >> d.rateNoB12 >> d.rate15pctHe8 >> d.rateNoHe8;
  }
}

void Li9Calc::initLivetimes()
{
  std::ifstream ifs(dataPath(LIVETIME_FILENAME));

  auto fill = [&](Site site, size_t ndet) {
    double tmp;
    for (size_t i = 0; i < ndet; ++i) {
      ifs >> tmp;
      adLivedays[site] += tmp;
    }
  };

  fill(Site::EH1, 2);
  fill(Site::EH2, 2);
  fill(Site::EH3, 4);
}

std::string Li9Calc::dataPath(const char* filename)
{
  std::string srcCodePath = __FILE__;
  std::string dir = srcCodePath.substr(0, srcCodePath.rfind('/'));
  return dir + "/../data/" + filename;
}

double Li9Calc::extrapolate(unsigned shower_pe,
                            std::function<double(unsigned)> getter)
{
  const unsigned delta = DELTA_VETO_BDRY_PE;
  const unsigned leftEdge = shower_pe / delta * delta;
  const unsigned rightEdge = leftEdge + delta;
  const double weight = double(shower_pe - leftEdge) / delta;

  return (1 - weight) * getter(leftEdge) + weight * getter(rightEdge);
}


double Li9Calc::measLowRange(Site site)
{
  return get(site, {MIN_PE, MAX_NTAG_PE}).nomRate;
}

double Li9Calc::measMidRange(Site site, unsigned shower_pe)
{
  auto getter = [&](unsigned edge_pe) -> double {
    return get(site, {MAX_NTAG_PE, edge_pe}).nomRate;
  };

  return extrapolate(shower_pe, getter);
}

double Li9Calc::measHighRange(Site site, unsigned shower_pe)
{
  auto getter = [&](unsigned edge_pe) -> double {
    return get(site, {edge_pe, MAX_PE}).nomRate;
  };

  return extrapolate(shower_pe, getter);
}

double Li9Calc::li9daily(Site site, unsigned shower_pe, double showerVeto_ms)
{
  const double totLow = measLowRange(site);
  const double totMid = measMidRange(site, shower_pe);
  const double totHigh = measHighRange(site, shower_pe);

  const double li9SelSurvProb = exp(-LI9_SEL_SHOWER_VETO_MS / LI9_LIVETIME_MS);
  const double ibdSelSurvProb = exp(-showerVeto_ms / LI9_LIVETIME_MS);

  const double predCount =
    1./NTAG_EFF * totLow +
    totMid +
    ibdSelSurvProb / li9SelSurvProb * totHigh;

  return predCount / adLivedays[site];
}
