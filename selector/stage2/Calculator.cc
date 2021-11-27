#include "Calculator.hh"

#include "CalcsTree.hh"
#include "MultCut.hh"
#include "MuonAlg.hh"
#include "Selectors.hh"
#include "SinglesCalc.hh"

#include "../common/Misc.hh"

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

double Calculator::li9Daily(Det detector)
{
  const Config* config = pipe.getTool<Config>();
  const unsigned shower_pe = config->get<double>("ibdShowerMuChgCut");
  const double showerVeto_ms = 1e-3 * config->get<double>("ibdShowerMuPostVeto_us");

  return li9calc.li9daily_linreg(site, shower_pe, showerVeto_ms);
}

double Calculator::li9DailyErr(Det detector)
{
  // XXX For now just use Chris's uncertainties for the nominal cuts. We should
  // try to be more precise.
  switch (site) {
  case Site::EH1:
    return 0.27 * li9Daily(detector);
  case Site::EH2:
    return 0.29 * li9Daily(detector);
  case Site::EH3:
    return 0.37 * li9Daily(detector);
  }

  throw;
}

void Calculator::writeDelayedEff(TreeWriter<CalcsTree>& w, Det detector, SinglesCalc& singCalc,
                                 Float_t vetoEffSingles, IbdSel* ibdSel)
{
  IbdSel* ibdSelLow = pipe.getAlg<IbdSel>(IbdSel::TAG(detector, TAG_LOW_DELAYED_EMIN));
  SingleSel* singleSelLow = pipe.getAlg<SingleSel>(SingleSel::TAG(detector, TAG_LOW_DELAYED_EMIN));
  TH1F* hSingLow = singleSelLow->hist;
  MultCutTool* multCutLow = pipe.getTool<MultCutTool>(TAG_LOW_DELAYED_EMIN);
  SinglesCalc singCalcLow(hSingLow, livetime_s(),
                          multCutLow->singleCuts, multCutLow->ibdCuts,
                          vetoEffSingles,
                          ibdSelLow->PROMPT_MIN, ibdSelLow->PROMPT_MAX,
                          ibdSelLow->DELAYED_MIN, ibdSelLow->DELAYED_MAX,
                          ibdSelLow->DT_MAX_US);

  // XXX just write singCalcLow.accDaily() etc. to the calcs tree then continue
  // to calculate the delayed cut eff in fit_prep so we can take advantage of
  // hadd etc.

  // auto h_ncap = std::make_unique<TH1F>("h_ncap", "h_ncap", 160, 4, 12);
  // auto h_ncap = std::unique_ptr<TH1F>((TH1F*)ibdSelLow->histD->Clone());
  auto h_ncap = std::unique_ptr<TH1F>
    ((TH1F*)hSingLow->Clone(LeakStr("h_ncap_ad%d", int(detector))));
  h_ncap->Reset();
  auto h_ibd = ibdSelLow->histD;
  for (int bin = h_ibd->FindBin(ibdSelLow->DELAYED_MIN);
       bin <= h_ibd->FindBin(ibdSelLow->DELAYED_MAX);
       ++bin)
    h_ncap->Fill(h_ibd->GetBinCenter(bin), h_ibd->GetBinContent(bin));


  auto* hSingLowD = (TH1F*) hSingLow->Clone();
  hSingLowD->Reset();

  for (int bin = hSingLow->FindBin(ibdSelLow->DELAYED_MIN);
       bin <= hSingLow->FindBin(ibdSelLow->DELAYED_MAX);
       ++bin)
    hSingLowD->Fill(hSingLow->GetBinCenter(bin), hSingLow->GetBinContent(bin));

  if (hSingLowD->Integral())
    hSingLowD->Scale(1 / hSingLowD->Integral());

  // NB we use singCalc.dmcEff, not singCalcLow.dmcEff, since we are predicting the raw measurement
  // and accDaily already corrects for singCalcLow.dmcEff
  const float accLow = singCalcLow.accDaily() * livetime_s() / 86400
    * singCalc.dmcEff() * vetoEff(detector);
  h_ncap->Add(hSingLowD, -accLow);

  w.data.delayedEffAbs = fine_integral(h_ncap.get(),
                                       ibdSel->DELAYED_MIN, ibdSel->DELAYED_MAX) /
    fine_integral(h_ncap.get(), ibdSelLow->DELAYED_MIN, ibdSelLow->DELAYED_MAX);

  w.data.delayedEffRel = fine_integral(h_ncap.get(),
                                       ibdSel->DELAYED_MIN, ibdSel->DELAYED_MAX) /
    fine_integral(h_ncap.get(), 6, 12);

  w.data.accDailyLowDelCut = singCalcLow.accDaily();

  pipe.getOutFile()->cd();
  h_ncap->Write();
}

void Calculator::writeEntry(TreeWriter<CalcsTree>& w, Det detector)
{
  IbdSel* ibdSel = pipe.getAlg<IbdSel>(detector);
  TH1F* hSing = pipe.getAlg<SingleSel>(detector)->hist;
  const double vetoEffSingles = vetoEff(detector, MuonAlg::Purpose::ForSingles);
  SinglesCalc singCalc(hSing, livetime_s(),
                       multCut->singleCuts, multCut->ibdCuts,
                       vetoEffSingles,
                       ibdSel->PROMPT_MIN, ibdSel->PROMPT_MAX,
                       ibdSel->DELAYED_MIN, ibdSel->DELAYED_MAX,
                       ibdSel->DT_MAX_US);

  w.data.phase = phase;
  w.data.seq = seq;
  w.data.site = site;

  w.data.livetime_s = livetime_s();

  w.data.detector = detector;
  w.data.vetoEff = vetoEff(detector);
  w.data.dmcEff = singCalc.dmcEff();
  w.data.accDaily = singCalc.accDaily();
  w.data.accDailyErr = singCalc.accDailyErr(site);
  w.data.li9Daily = li9Daily(detector);
  w.data.li9DailyErr = li9DailyErr(detector);

  // For diagnostics and ReCalc
  w.data.nPreMuons = singCalc.nPreMuons();
  w.data.nPlusLikeSingles = singCalc.nPlusLikeSingles();
  w.data.nPromptLikeSingles = singCalc.nPromptLikeSingles();
  w.data.nDelayedLikeSingles = singCalc.nDelayedLikeSingles();
  w.data.preMuonHz = singCalc.preMuonHz();
  w.data.plusLikeHz = singCalc.plusLikeHz();
  w.data.promptLikeHz = singCalc.promptLikeHz();
  w.data.delayedLikeHz = singCalc.delayedLikeHz();
  w.data.vetoEffSingles = vetoEffSingles;
  w.data.dmcEffSingles = singCalc.dmcEffSingles();

  writeDelayedEff(w, detector, singCalc, vetoEffSingles, ibdSel);

  w.fill();
}

void Calculator::writeEntries(const char* treename)
{
  TreeWriter<CalcsTree> w(treename);
  w.connect(pipe);

  for (Det detector : util::ADsFor(site, phase))
    writeEntry(w, detector);
}

double fine_integral(TH1* h, double x1, double x2)
{
  x1 = x1 > h->GetXaxis()->GetXmin() ? x1 : h->GetXaxis()->GetXmin();
  x2 = x2 < h->GetXaxis()->GetXmax() ? x2 : h->GetXaxis()->GetXmax();

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
