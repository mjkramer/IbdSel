#include "Calculator.hh"

#include "CalcsTree.hh"
#include "MultCut.hh"
#include "MuonAlg.hh"
#include "Selectors.hh"

#include "../common/Misc.hh"

#include "external/FukushimaLambertW.hh"

#include "../SelectorFramework/core/Assert.hh"
#include "../SelectorFramework/core/Kernel.hh"

#include <TH1F.h>

#include <cmath>

Calculator::Calculator(Pipeline& pipe, Site site, Phase phase, UInt_t seq) :
  pipe(pipe), site(site), phase(phase), seq(seq)
{
  multCut = pipe.getTool<MultCutTool>();
}

double Calculator::livetime_s()
{
  const auto h = (TH1F*) pipe.inFile()->Get(keys::HistLivetime);
  return h->GetBinContent(1);
}

double Calculator::vetoEff(Det detector, MuonAlg::Purpose purp)
{
  const auto muonAlg = pipe.getAlg<MuonAlg>(purp);
  const double veto_s = muonAlg->vetoTime_s(detector);

  return 1 - veto_s / livetime_s();
}

TH1F* Calculator::singlesHist(Det detector)
{
  const auto alg = pipe.getAlg<SingleSel>(detector);
  return alg->hist;
}

double Calculator::singlesCount(Det detector,
                                double lowE,
                                std::optional<double> optUpperE)
{
  const auto h = singlesHist(detector);
  const int lowBin = h->FindBin(lowE);
  // Assuming we're using 0.1 MeV bins, and optUpperE is a multiple of 0.1 MeV,
  // FindBin will give us the bin whose low edge is optUpperE, which means we'd
  // include [optUpperE, optUpperE + 0.1) in the integral, which we don't want!
  // Thus we must subtract 1 from the bin#.
  const int highBin = optUpperE ? h->FindBin(*optUpperE) - 1 :
    h->GetNbinsX() + 1;         // no upper limit? then include overflow bin

  return h->Integral(lowBin, highBin);
}

double Calculator::calcSinglesHz(Det detector, double N)
{
  const double eMuSingles = vetoEff(detector, MuonAlg::Purpose::ForSingles);
  const double eDmcSingles = dmcEffSingles(detector);
  const double T = livetime_s();

  return N / (eDmcSingles * eMuSingles * T);
}

// XXX get rid of me
double Calculator::nPreMuons(Det detector)
{
  return singlesCount(detector, IbdSel::PROMPT_MAX);
}

double Calculator::nPlusLikeSingles(Det detector)
{
  return singlesCount(detector, IbdSel::PROMPT_MIN);
}

double Calculator::nPromptLikeSingles(Det detector)
{
  return singlesCount(detector, IbdSel::PROMPT_MIN, IbdSel::PROMPT_MAX);
}

double Calculator::nDelayedLikeSingles(Det detector)
{
  return singlesCount(detector, IbdSel::DELAYED_MIN, IbdSel::DELAYED_MAX);
}

double Calculator::dmcEffSingles(Det detector)
{
  const MultCutTool::Cuts& cuts = multCut->singleCuts;

  const double nBefore = singlesCount(detector,
                                      cuts.emin_before, cuts.emax_before);
  const double nAfter = singlesCount(detector,
                                     cuts.emin_after, cuts.emax_after);
  const double tBefore = 1e-6 * cuts.usec_before;
  const double tAfter = 1e-6 * cuts.usec_after;

  const double eMuSingles = vetoEff(detector, MuonAlg::Purpose::ForSingles);
  const double T = livetime_s();

  const double arg =
    ((nBefore * tBefore) + (nAfter * tAfter)) / (eMuSingles * T);
  return exp(Fukushima::LambertW0(-arg));
}

double Calculator::singlesHz(Det detector,
                             double lowE,
                             std::optional<double> optUpperE)
{
  const double N = singlesCount(detector, lowE, optUpperE);
  return calcSinglesHz(detector, N);
}

double Calculator::preMuonHz(Det detector)
{
  const double N = nPreMuons(detector);
  return calcSinglesHz(detector, N);
}

double Calculator::promptLikeHz(Det detector)
{
  const double N = nPromptLikeSingles(detector);
  return calcSinglesHz(detector, N);
}

double Calculator::plusLikeHz(Det detector)
{
  const double N = nPlusLikeSingles(detector);
  return calcSinglesHz(detector, N);
}

double Calculator::delayedLikeHz(Det detector)
{
  const double N = nDelayedLikeSingles(detector);
  return calcSinglesHz(detector, N);
}

double Calculator::dmcEff(Det detector)
{
  const MultCutTool::Cuts& cuts = multCut->ibdCuts;

  const double rBefore = singlesHz(detector,
                                   cuts.emin_before, cuts.emax_before);
  const double rAfter = singlesHz(detector,
                                  cuts.emin_after, cuts.emax_after);
  const double tBefore = 1e-6 * cuts.usec_before;
  const double tAfter = 1e-6 * cuts.usec_after;

  const double arg = (rBefore * tBefore) + (rAfter * tAfter);
  return exp(-arg);
}

// Get the rate for singles that lie in [keep_min, keep_max]
// but do NOT lie in [drop_min, drop_max]
double Calculator::subtractSinglesHz(Det detector,
                                     float keep_min, float keep_max,
                                     float drop_min, float drop_max)
{
  float result = 0;

  if (keep_min < drop_min) {
    float top = keep_max < drop_min ? keep_max : drop_min;
    result += singlesHz(detector, keep_min, top);
  }

  if (keep_max > drop_max) {
    float bottom = keep_min > drop_max ? keep_min : drop_max;
    result += singlesHz(detector, bottom, keep_max);
  }

  return result;
}

// This rate (as for all bkgs) is "ideal", meaning we need to multiply by mu/dmc
// eff before subtracting from raw spectrum i.e. we DON'T need to multiply by
// mu/dmc eff before subtracting from corrected spectrum. This is the convention
// used in the fitter's "Theta13" input file.
double Calculator::accDaily(Det detector)
{
  const MultCutTool::Cuts& cuts = multCut->ibdCuts;

  const double rPrompt = promptLikeHz(detector);
  const double rDelayed = delayedLikeHz(detector);
  const double rBefore = singlesHz(detector,
                                   cuts.emin_before, cuts.emax_before);
  const double rAfter = singlesHz(detector,
                                  cuts.emin_after, cuts.emax_after);

  // BBNP => Before-like But Not Prompt-like
  // Needed since our calculation assumes independent Poisson processes
  const double rBBNP =
    subtractSinglesHz(detector, cuts.emin_before, cuts.emax_before,
                      IbdSel::PROMPT_MIN, IbdSel::PROMPT_MAX);

  const double promptWindow = 1e-6 * IbdSel::DT_MAX_US;
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

  return R / dmcEff(detector) * units::hzToDaily; // hz * 86_400
}

double Calculator::accDailyErr(Det detector)
{
  // TODO implement proper statistical calculation. For now, use empirical
  // formula based on old LBNL selection (assuming full day's worth of
  // statistics). In the final fit, we will probably just override the
  // statistical uncertainty with a 1% systematic based on IHEP's comparisons of
  // different methods for calculating the accidentals rate.

  const double rate = accDaily(detector);

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

double Calculator::li9Daily(Det detector)
{
  const Config* config = pipe.getTool<Config>();
  const unsigned shower_pe = config->get<double>("ibdShowerMuChgCut");
  const double showerVeto_ms = 1e-3 * config->get<double>("ibdShowerMuPostVeto_us");

  return li9calc.li9daily(site, shower_pe, showerVeto_ms);
}

double Calculator::li9DailyErr(Det detector)
{
  return 0.3 * li9Daily(detector);
}

void Calculator::writeEntry(TreeWriter<CalcsTree>& w, Det detector)
{
  w.data.phase = phase;
  w.data.seq = seq;
  w.data.site = site;

  w.data.livetime_s = livetime_s();

  w.data.detector = detector;
  w.data.vetoEff = vetoEff(detector);
  w.data.dmcEff = dmcEff(detector);
  w.data.accDaily = accDaily(detector);
  w.data.accDailyErr = accDailyErr(detector);
  w.data.li9Daily = li9Daily(detector);
  w.data.li9DailyErr = li9DailyErr(detector);

  // For diagnostics and ReCalc
  w.data.nPreMuons = nPreMuons(detector);
  w.data.nPlusLikeSingles = nPlusLikeSingles(detector);
  w.data.nPromptLikeSingles = nPromptLikeSingles(detector);
  w.data.nDelayedLikeSingles = nDelayedLikeSingles(detector);
  w.data.preMuonHz = preMuonHz(detector);
  w.data.plusLikeHz = plusLikeHz(detector);
  w.data.promptLikeHz = promptLikeHz(detector);
  w.data.delayedLikeHz = delayedLikeHz(detector);
  w.data.vetoEffSingles = vetoEff(detector, MuonAlg::Purpose::ForSingles);
  w.data.dmcEffSingles = dmcEffSingles(detector);

  w.fill();
}

void Calculator::writeEntries(const char* treename)
{
  TreeWriter<CalcsTree> w(treename);
  w.connect(pipe);

  for (Det detector : util::ADsFor(site, phase))
    writeEntry(w, detector);
}
