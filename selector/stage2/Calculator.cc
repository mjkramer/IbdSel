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
  singleSel = pipe.getAlg<SingleSel>();
  ibdSel = pipe.getAlg<IbdSel>();
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

void Calculator::writeEntry(TreeWriter<CalcsTree>& w, Det detector)
{
  TH1F* hSing = pipe.getAlg<SingleSel>(detector)->hist;
  const double vetoEffSingles = vetoEff(detector, MuonAlg::Purpose::ForSingles);
  SinglesCalc singCalc(hSing, vetoEffSingles, livetime_s(),
                       multCut->singleCuts, vetoEffSingles,
                       ibdSel->PROMPT_MIN, ibdSel->PROMPT_MAX,
                       ibdSel->DELAYED_MIN, ibdSel->DELAYED_MAX,
                       ibdSel->DT_MAX_US);

  w.data.phase = phase;
  w.data.seq = seq;
  w.data.site = site;

  w.data.livetime_s = livetime_s();

  w.data.detector = detector;
  w.data.vetoEff = vetoEff(detector);
  w.data.dmcEff = singCalc.dmcEff(multCut->ibdCuts);
  w.data.accDaily = singCalc.accDaily(multCut->ibdCuts);
  w.data.accDailyErr = singCalc.accDailyErr(multCut->ibdCuts, site);
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

  w.fill();
}

void Calculator::writeEntries(const char* treename)
{
  TreeWriter<CalcsTree> w(treename);
  w.connect(pipe);

  for (Det detector : util::ADsFor(site, phase))
    writeEntry(w, detector);
}
