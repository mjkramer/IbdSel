#include "Li9Calc.hh"

#include <TGraph.h>
#include <TF1.h>

#include <fstream>

Li9Calc::Li9Calc() :
  mode(Mode::Nominal)
{
  initTable();
}

void Li9Calc::setMode(Mode mode)
{
  this->mode = mode;
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
    ifs >> d.nomRate >> d.statUnc >> d.chi2 >> d.rateNoB12 >> d.rate15pctHe8 >> d.rateNoHe8;
  }
}

std::string Li9Calc::dataPath(const char* filename)
{
  std::string base = IBDSEL_BASE;
  return base + "/" + DATA_DIR + "/" + filename;
}

double Li9Calc::get(Site site, MuonBin bin)
{
  Data& d = table[site][bin];

  switch (mode) {
  case Mode::Nominal:
    return d.nomRate;
  case Mode::NoB12:
    return d.rateNoB12;
  case Mode::Fix15pctHe8:
    return d.rate15pctHe8;
  case Mode::NoHe8:
    return d.rateNoHe8;
  default:
    throw;
  }
}

double Li9Calc::interpolate(unsigned shower_pe,
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
  return get(site, {MIN_PE, MAX_NTAG_PE});
}

double Li9Calc::measMidRange(Site site, unsigned shower_pe)
{
  auto getter = [&](unsigned edge_pe) -> double {
    return get(site, {MAX_NTAG_PE, edge_pe});
  };

  return interpolate(shower_pe, getter);
}

double Li9Calc::measHighRange(Site site, unsigned shower_pe)
{
  auto getter = [&](unsigned edge_pe) -> double {
    return get(site, {edge_pe, MAX_PE});
  };

  return interpolate(shower_pe, getter);
}

// Note: Shower veto is now NOT applied in Li9 selection
double Li9Calc::li9daily_nearest(Site site, double shower_pe, double showerVeto_ms)
{
  // n-tagged; won't be affected by shower veto; 8/6 MeV near/far prompt cut:
  const double totLow = measLowRange(site);
  // not n-tagged; won't be affected by shower veto; 8/6 MeV near/far prompt cut:
  const double totMid = measMidRange(site, shower_pe);
  // not n-tagged; WILL be partially removed by shower veto; 3.5 MeV prompt cut:
  const double totHigh = measHighRange(site, shower_pe);

  const double showerVetoSurvProb = exp(-showerVeto_ms / LI9_LIFETIME_MS);
  const double lowPePromptEff =
    site == Site::EH3 ? FAR_LOW_PE_PROMPT_EFF : NEAR_LOW_PE_PROMPT_EFF;

  const size_t isite = int(site) - 1;
  const double denom = LIVEDAYS[isite] * NDET_WEIGHTED[isite] *
    VETO_EFFS[isite] * MULT_EFFS[isite];

  const double predCount =
    totLow / lowPePromptEff / NTAG_EFF +
    totMid / lowPePromptEff +
    totHigh / HIGH_PE_PROMPT_EFF * showerVetoSurvProb;

  return predCount / denom;
}

double Li9Calc::li9daily_linreg(Site site, double shower_pe, double showerVeto_ms)
{
  const int N = (MAX_VETO_BDRY_PE - MIN_VETO_BDRY_PE) / DELTA_VETO_BDRY_PE + 1;

  double cuts[N];
  double rates[N];

  for (int i = 0; i < N; ++i) {
    cuts[i] = MIN_VETO_BDRY_PE + i*DELTA_VETO_BDRY_PE;
    rates[i] = li9daily_nearest(site, cuts[i], showerVeto_ms);
  }

  TGraph graph(N, cuts, rates);
  graph.Fit("pol1", "0Q");
  return graph.GetFunction("pol1")->Eval(shower_pe);
}
