#pragma once

#include "MultCut.hh"

#include <optional>

class TH1F;

class SinglesCalc {
public:
  SinglesCalc(TH1F* hSing, double eMu, double livetime_s,
              MultCutTool::Cuts singleMultCuts,
              MultCutTool::Cuts ibdMultCuts,
              double eMuSingles,
              double promptMin, double promptMax,
              double delayedMin, double delayedMax,
              double dt_max_us);

  double dmcEff();
  double accDaily();
  double accDailyErr(Site site);
  double nPreMuons();
  double nPlusLikeSingles();
  double nPromptLikeSingles();
  double nDelayedLikeSingles();
  double preMuonHz();
  double plusLikeHz();
  double promptLikeHz();
  double delayedLikeHz();
  double dmcEffSingles();

  double singlesCount(double lowE,
                      std::optional<double> optUpperE = std::nullopt);

  double singlesHz(double lowE,
                   std::optional<double> optUpperE = std::nullopt);

private:
  double calcSinglesHz(double N);
  double subtractSinglesHz(float keep_min, float keep_max,
                           float drop_min, float drop_max);

  TH1F* hSing;
  double eMu;
  double livetime_s;
  MultCutTool::Cuts singleMultCuts, ibdMultCuts;
  double eMuSingles;
  double promptMin, promptMax;
  double delayedMin, delayedMax;
  double dt_max_us;
};
