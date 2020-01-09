#pragma once

#include "Constants.cc"
#include "Selectors.cc"
#include "MuonAlg.cc"
#include "MultCut.cc"
#include "CalcsTree.cc"
#include "Misc.cc"

#include "external/FukushimaLambertW.cc"

#include "SelectorFramework/core/Kernel.cc"

#include <cmath>

class Calculator {
public:
  Calculator(Pipeline& pipe, Stage stage, UInt_t seq, Site site) :
    pipe(pipe), stage(stage), seq(seq), site(site) {}

  double livetime_s();
  double vetoEff(Det detector, MuonAlg::Purpose purp = MuonAlg::Purpose::ForIBDs);
  double dmcEff(Det detector);

  double accDaily(Det detector);
  double accDailyErr(Det detector);
  double li9Daily(Det detector);
  double li9DailyErr(Det detector);

  void writeValues();

private:
  TH1F* singlesHist(Det detector);
  double nPlusLikeSingles(Det detector);
  double nPromptLikeSingles(Det detector);
  double nDelayedLikeSingles(Det detector);
  double singlesHz(Det detector, double N);
  double promptLikeHz(Det detector);
  double plusLikeHz(Det detector);
  double delayedLikeHz(Det detector);
  double dmcEffSingles(Det detector);

  Pipeline& pipe;
  Stage stage;
  UInt_t seq;
  Site site;
};

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

double Calculator::nPlusLikeSingles(Det detector)
{
  const auto h = singlesHist(detector);
  // Include overflow bin:
  return h->Integral(h->FindBin(0.7), h->GetNbinsX() + 1);
}

double Calculator::nPromptLikeSingles(Det detector)
{
  const auto h = singlesHist(detector);
  return h->Integral(h->FindBin(0.7), h->FindBin(12));
}

double Calculator::nDelayedLikeSingles(Det detector)
{
  const auto h = singlesHist(detector);
  return h->Integral(h->FindBin(6), h->FindBin(12));
}

double Calculator::dmcEffSingles(Det detector)
{
  const double nPlus = nPlusLikeSingles(detector);
  const double t = 1e-6 * (MultCutTool::SINGLE_USEC_BEFORE +
                           MultCutTool::SINGLE_USEC_AFTER);
  const double eMuSingles = vetoEff(detector, MuonAlg::Purpose::ForSingles);
  const double T = livetime_s();

  const double arg = (nPlus * t) / (eMuSingles * T);
  return exp(Fukushima::LambertW0(-arg));
}

double Calculator::singlesHz(Det detector, double N)
{
  const double eMuSingles = vetoEff(detector, MuonAlg::Purpose::ForSingles);
  const double eDmcSingles = dmcEffSingles(detector);
  const double T = livetime_s();

  return N / (eDmcSingles * eMuSingles * T);
}

double Calculator::promptLikeHz(Det detector)
{
  const double N = nPromptLikeSingles(detector);
  return singlesHz(detector, N);
}

double Calculator::plusLikeHz(Det detector)
{
  const double N = nPlusLikeSingles(detector);
  return singlesHz(detector, N);
}

double Calculator::delayedLikeHz(Det detector)
{
  const double N = nDelayedLikeSingles(detector);
  return singlesHz(detector, N);
}

double Calculator::dmcEff(Det detector)
{
  const double tL = 1e-6 * MultCutTool::IBD_USEC_BEFORE;
  const double tR = 1e-6 * MultCutTool::IBD_USEC_AFTER;
  const double Rplu = plusLikeHz(detector);
  const double Rd = delayedLikeHz(detector);

  const double arg = (Rplu * tL) + (Rd * tR);
  return exp(-arg);
}

// This rate (as for all bkgs) is "ideal", meaning we need to multiply by mu/dmc
// eff before subtracting from raw spectrum i.e. we DON'T need to multiply by
// mu/dmc eff before subtracting from corrected spectrum. This is the convention
// used in the fitter's "Theta13" input file.
double Calculator::accDaily(Det detector)
{
  const double Rpro = promptLikeHz(detector);
  const double Rplu = plusLikeHz(detector);
  const double Rd = delayedLikeHz(detector);

  const double promptWindow = 1e-6 * IbdSel::DT_MAX_US;
  const double prePromptWindowEmptyWindow =
    (1e-6 * MultCutTool::IBD_USEC_BEFORE) - promptWindow;
  const double postDelayedEmptyWindow = 1e-6 * MultCutTool::IBD_USEC_AFTER;

  // sanity checks, assuming standard DMC
  assert(promptWindow == 200e-6);
  assert(prePromptWindowEmptyWindow == 200e-6);
  assert(postDelayedEmptyWindow == 200e-6);

  const double probOnePro = (Rpro * promptWindow) * exp(-Rpro * promptWindow);
  const double probZeroPlu = exp(-Rplu * prePromptWindowEmptyWindow);
  const double probZeroD = exp(-Rd * postDelayedEmptyWindow);

  // XXX In principle we should account for the dt > 1 requirement or else
  // remove that cut. Extra factor ~ exp(-20 * 1e-6) = 0.99998 meh who cares
  const double R = Rd * probOnePro * probZeroPlu * probZeroD;

  // This R assumes veto eff. of 1 (i.e. uncorrected for it) while R IS
  // corrected for DMC eff by construction (i.e. we are predicting what we'd see
  // in the RAW spectrum, modulo veto eff.). In the fitter, we multiply every
  // bkg rate by the veto and DMC efficiencies (*for IBDs*), prior to
  // subtracting from the raw IBD spectrum(?). Therefore, here we must divide by
  // the DMC efficiency for IBDs, to pre-cancel-out this multiplication.

  return R / dmcEff(detector) * units::hzToDaily; // hz * 86_400
}

double Calculator::accDailyErr(Det detector)
{
  // Conservative estimate
  return 0.01 * accDaily(detector);
}

// TODO: Use the (IBD) muon veto parameters to get li9 rate
double Calculator::li9Daily(Det detector)
{
  return 1;                     // XXX
}

double Calculator::li9DailyErr(Det detector)
{
  return 0.5;                   // XXX
}

void Calculator::writeValues()
{
  TreeWriter<CalcsTree> w("results");
  w.connect(pipe);

  w.data.stage = stage;
  w.data.seq = seq;
  w.data.site = site;

  w.data.livetime_s = livetime_s();

  for (Det detector : util::ADsFor(site, stage)) {
    w.data.detector = detector;
    w.data.vetoEff = vetoEff(detector);
    w.data.dmcEff = dmcEff(detector);
    w.data.accDaily = accDaily(detector);
    w.data.accDailyErr = accDailyErr(detector);
    w.data.li9Daily = li9Daily(detector);
    w.data.li9DailyErr = li9DailyErr(detector);

    // XXX diagnostics
    w.data.nPlusLikeSingles = nPlusLikeSingles(detector);
    w.data.nPromptLikeSingles = nPromptLikeSingles(detector);
    w.data.nDelayedLikeSingles = nDelayedLikeSingles(detector);
    w.data.promptLikeHz = promptLikeHz(detector);
    w.data.delayedLikeHz = delayedLikeHz(detector);
    w.data.dmcEffSingles = dmcEffSingles(detector);

    w.fill();
  }
}
