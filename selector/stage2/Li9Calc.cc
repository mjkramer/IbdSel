#include "Li9Calc.hh"

#include <fstream>

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
