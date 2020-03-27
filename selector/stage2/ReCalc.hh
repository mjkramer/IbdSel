#pragma once

#include "Calculator.hh"
#include "CalcsTree.hh"

#include "../SelectorFramework/core/DirectReader.hh"
#include "../SelectorFramework/core/TreeWriter.hh"

class ReCalc : public Calculator {
public:
  ReCalc(Pipeline& pipe, Site site, Phase phase);

  void writeEntries(const char* treename = "results_recalc") override;

  double livetime_s() override;
  double vetoEff(Det detector,
                 MuonAlg::Purpose purp = MuonAlg::Purpose::ForIBDs) override;

private:
  double nPreMuons(Det detector) override;
  double nPlusLikeSingles(Det detector) override;
  double nPromptLikeSingles(Det detector) override;
  double nDelayedLikeSingles(Det detector) override;

  DirectReader<CalcsTree> reader;
};
