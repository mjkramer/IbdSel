#include "SinglesCalc.hh"

#include "external/FukushimaLambertW.hh"


#include <TH1F.h>

static double fine_integral(TH1F* h, double x1, double x2);

SinglesCalc::SinglesCalc(TH1F* hSing, double livetime_s,
                         MultCutTool::Cuts singleMultCuts,
                         MultCutTool::Cuts ibdMultCuts,
                         double eMuSingles,
                         double promptMin, double promptMax,
                         double delayedMin, double delayedMax,
                         double dt_max_us) :
  hSing(hSing), eMu(eMu), livetime_s(livetime_s),
  singleMultCuts(singleMultCuts),
  ibdMultCuts(ibdMultCuts),
  eMuSingles(eMuSingles),
  promptMin(promptMin), promptMax(promptMax),
  delayedMin(delayedMin), delayedMax(delayedMax),
  dt_max_us(dt_max_us)
{
}


double SinglesCalc::singlesCount(double lowE,
                                 std::optional<double> optUpperE)
{
  // const int lowBin = hSing->FindBin(lowE);
  // Assuming we're using 0.1 MeV bins, and optUpperE is a multiple of 0.1 MeV,
  // FindBin will give us the bin whose low edge is optUpperE, which means we'd
  // include [optUpperE, optUpperE + 0.1) in the integral, which we don't want!
  // Thus we must subtract 1 from the bin#.
  // const int highBin = optUpperE ? hSing->FindBin(*optUpperE) - 1 :
  //   hSing->GetNbinsX() + 1;         // no upper limit? then include overflow bin

  // XXX this is wrong if the cuts do not align with the bins, see fine_integral
  // in misc_ana/AccUncMC
  // return hSing->Integral(lowBin, highBin);

  const double maxE = hSing->GetXaxis()->GetXmax();
  return fine_integral(hSing, lowE, optUpperE.value_or(maxE));
}


double SinglesCalc::calcSinglesHz(double N)
{
  // previously was using eMu not eMuSingles
  // could this have caused buggy results when varying muon veto?
  // XXX check accidentals rates / DMC effs on those theta13 files
  return N / (dmcEffSingles() * eMuSingles * livetime_s);
}

// XXX get rid of me
double SinglesCalc::nPreMuons()
{
  return singlesCount(promptMax);
}

double SinglesCalc::nPlusLikeSingles()
{
  return singlesCount(promptMin);
}

double SinglesCalc::nPromptLikeSingles()
{
  return singlesCount(promptMin, promptMax);
}

double SinglesCalc::nDelayedLikeSingles()
{
  return singlesCount(delayedMin, delayedMax);
}

double SinglesCalc::dmcEffSingles()
{
  const MultCutTool::Cuts& cuts = singleMultCuts;

  const double nBefore = singlesCount(cuts.emin_before, cuts.emax_before);
  const double nAfter = singlesCount(cuts.emin_after, cuts.emax_after);
  const double tBefore = 1e-6 * cuts.usec_before;
  const double tAfter = 1e-6 * cuts.usec_after;

  const double T = livetime_s;

  const double arg =
    ((nBefore * tBefore) + (nAfter * tAfter)) / (eMuSingles * T);
  return exp(Fukushima::LambertW0(-arg));
}

double SinglesCalc::singlesHz(double lowE,
                              std::optional<double> optUpperE)
{
  const double N = singlesCount(lowE, optUpperE);
  return calcSinglesHz(N);
}

double SinglesCalc::preMuonHz()
{
  const double N = nPreMuons();
  return calcSinglesHz(N);
}

double SinglesCalc::promptLikeHz()
{
  const double N = nPromptLikeSingles();
  return calcSinglesHz(N);
}

double SinglesCalc::plusLikeHz()
{
  const double N = nPlusLikeSingles();
  return calcSinglesHz(N);
}

double SinglesCalc::delayedLikeHz()
{
  const double N = nDelayedLikeSingles();
  return calcSinglesHz(N);
}

double SinglesCalc::dmcEff()
{
  const MultCutTool::Cuts& cuts = ibdMultCuts;

  const double rBefore = singlesHz(cuts.emin_before, cuts.emax_before);
  const double rAfter = singlesHz(cuts.emin_after, cuts.emax_after);
  const double tBefore = 1e-6 * cuts.usec_before;
  const double tAfter = 1e-6 * cuts.usec_after;

  const double arg = (rBefore * tBefore) + (rAfter * tAfter);
  return exp(-arg);
}

// Get the rate for singles that lie in [keep_min, keep_max]
// but do NOT lie in [drop_min, drop_max]
double SinglesCalc::subtractSinglesHz(float keep_min, float keep_max,
                                      float drop_min, float drop_max)
{
  float result = 0;

  if (keep_min < drop_min) {
    float top = keep_max < drop_min ? keep_max : drop_min;
    result += singlesHz(keep_min, top);
  }

  if (keep_max > drop_max) {
    float bottom = keep_min > drop_max ? keep_min : drop_max;
    result += singlesHz(bottom, keep_max);
  }

  return result;
}

// This rate (as for all bkgs) is "ideal", meaning we need to multiply by mu/dmc
// eff before subtracting from raw spectrum i.e. we DON'T need to multiply by
// mu/dmc eff before subtracting from corrected spectrum. This is the convention
// used in the fitter's "Theta13" input file.
double SinglesCalc::accDaily()
{
  const MultCutTool::Cuts& cuts = ibdMultCuts;

  const double rPrompt = promptLikeHz();
  const double rDelayed = delayedLikeHz();
  const double rBefore = singlesHz(cuts.emin_before, cuts.emax_before);
  const double rAfter = singlesHz(cuts.emin_after, cuts.emax_after);

  // BBNP => Before-like But Not Prompt-like
  // Needed since our calculation assumes independent Poisson processes
  // XXX isn't this normally just preMuonHz?
  const double rBBNP =
    subtractSinglesHz(cuts.emin_before, cuts.emax_before,
                      promptMin, promptMax);

  const double promptWindow = 1e-6 * dt_max_us;
  const double prePromptWindowEmptyWindow =
    (1e-6 * cuts.usec_before) - promptWindow;
  const double postDelayedEmptyWindow = 1e-6 * cuts.usec_after;

  const double probOnePrompt = ((rPrompt * promptWindow) *
                                exp(-rPrompt * promptWindow));
  const double probZeroBBNP = exp(-rBBNP * promptWindow);
  const double probZeroBefore = exp(-rBefore * prePromptWindowEmptyWindow);
  const double probZeroAfter = exp(-rAfter * postDelayedEmptyWindow);

  // XXX In principle we should account for the dt > 1 requirement or else
  // remove that cut. Extra factor ~ exp(-20 * 1e-6) = 0.99998 meh who cares
  const double R = rDelayed * probOnePrompt * probZeroBBNP *
    probZeroBefore * probZeroAfter;

  // This R assumes veto eff. of 1 (i.e. unreduced by it) while R IS
  // reduced by DMC eff by construction (i.e. we are predicting what we'd see
  // in the RAW spectrum, modulo veto eff.). In the fitter, we multiply every
  // bkg rate by the veto and DMC efficiencies (*for IBDs*), prior to
  // subtracting from the raw IBD spectrum(?). Therefore, here we must divide by
  // the DMC efficiency for IBDs, to pre-cancel-out this multiplication.

  return R / dmcEff() * units::hzToDaily; // hz * 86_400
}

double SinglesCalc::accDailyErr(Site site)
{
  // TODO implement proper statistical calculation. For now, use empirical
  // formula based on old LBNL selection (assuming full day's worth of
  // statistics). In the final fit, we will probably just override the
  // statistical uncertainty with a 1% systematic based on IHEP's comparisons of
  // different methods for calculating the accidentals rate.

  const double rate = accDaily();

  switch (site) {
  case Site::EH1:
    return 0.04 * rate;
  case Site::EH2:
    return 0.05 * rate;
  case Site::EH3:
    return 0.10 * rate;
  }

  throw;
}

static double fine_integral(TH1F* h, double x1, double x2)
{
  const int bin1 = h->FindBin(x1);
  const double frac1 =
    1 - ((x1 - h->GetBinLowEdge(bin1)) / h->GetBinWidth(bin1));

  const int bin2 = h->FindBin(x2);
  const double frac2 =
    bin2 == bin1
    ? -(1 - (x2 - h->GetBinLowEdge(bin2)) / h->GetBinWidth(bin2))
    : (x2 - h->GetBinLowEdge(bin2)) / h->GetBinWidth(bin2);

  const double middle_integral =
    bin2 - bin1 < 2
    ? 0
    : h->Integral(bin1 + 1, bin2 - 1);

  return
    frac1 * h->GetBinContent(bin1) +
    middle_integral +
    frac2 * h->GetBinContent(bin2);
}
