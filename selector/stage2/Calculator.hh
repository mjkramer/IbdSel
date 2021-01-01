#pragma once

#include "Li9Calc.hh"
#include "MuonAlg.hh"

#include "../common/Constants.hh"

class CalcsTree;
class MultCutTool;
class SingleSel;
class IbdSel;
template <class T> class TreeWriter;
class TH1F;

class Calculator {
public:
  Calculator(Pipeline& pipe, Site site, Phase phase, UInt_t seq);

  // Mark as 'virtual' those functions that use the Pipeline to access other
  // algs (except Config) and/or the stage1 input file. This way we can override
  // them in ReCalc.

  virtual double livetime_s();
  virtual double vetoEff(Det detector,
                         MuonAlg::Purpose purp = MuonAlg::Purpose::ForIBDs);

  double li9Daily(Det detector);
  double li9DailyErr(Det detector);

  void writeEntry(TreeWriter<CalcsTree>& w, Det detector);
  virtual void writeEntries(const char* treename = "results");

protected:
  Pipeline& pipe;
  Site site;
  Phase phase;
  UInt_t seq;

  Li9Calc li9calc;
  MultCutTool* multCut = nullptr;
  SingleSel* singleSel = nullptr;
  IbdSel* ibdSel = nullptr;
};

