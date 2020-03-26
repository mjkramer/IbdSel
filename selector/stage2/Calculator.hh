#pragma once

#include "Li9Calc.hh"
#include "MuonAlg.hh"

#include "../common/Constants.hh"

class TH1F;

class Calculator {
public:
  Calculator(Pipeline& pipe, Phase phase, UInt_t seq, Site site) :
    pipe(pipe), phase(phase), seq(seq), site(site) {}

  // Mark as 'virtual' those functions that use the Pipeline to access other
  // algs (except Config) and/or the stage1 input file. This way we can override
  // them in ReCalc.

  virtual double livetime_s();
  virtual double vetoEff(Det detector, MuonAlg::Purpose purp = MuonAlg::Purpose::ForIBDs);
  double dmcEff(Det detector);

  double accDaily(Det detector);
  double accDailyErr(Det detector);
  double li9Daily(Det detector);
  double li9DailyErr(Det detector);

  void writeValues();

private:
  virtual TH1F* singlesHist(Det detector);
  double nPreMuons(Det detector);
  double singlesIntegral(Det detector,
                         double lowE,
                         std::optional<double> optUpperE = std::nullopt);
  double nPlusLikeSingles(Det detector);
  double nPromptLikeSingles(Det detector);
  double nDelayedLikeSingles(Det detector);
  double singlesHz(Det detector, double N);
  double preMuonHz(Det detector);
  double promptLikeHz(Det detector);
  double plusLikeHz(Det detector);
  double delayedLikeHz(Det detector);
  double dmcEffSingles(Det detector);

  Pipeline& pipe;
  Phase phase;
  UInt_t seq;
  Site site;

  Li9Calc li9calc;
};

