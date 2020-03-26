#pragma once

#include "Li9Calc.hh"
#include "MuonAlg.hh"

#include "../common/Constants.hh"

class TH1F;

class Calculator {
public:
  Calculator(Pipeline& pipe, Phase phase, UInt_t seq, Site site) :
    pipe(pipe), phase(phase), seq(seq), site(site) {}

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
  double singlesIntegral(Det detector,
                         double lowE,
                         std::optional<double> optUpperE = std::nullopt);
  double singlesHz(Det detector, double N);

  double dmcEffSingles(Det detector);

  double nPreMuons(Det detector);
  double nPlusLikeSingles(Det detector);
  double nPromptLikeSingles(Det detector);
  double nDelayedLikeSingles(Det detector);

  double preMuonHz(Det detector);
  double plusLikeHz(Det detector);
  double promptLikeHz(Det detector);
  double delayedLikeHz(Det detector);

  Pipeline& pipe;
  Phase phase;
  UInt_t seq;
  Site site;

  Li9Calc li9calc;
};

