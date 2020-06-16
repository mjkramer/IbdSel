// NOTE: The recalc code is currently unused and untested. The idea is to re-run
// the calculations in Calculator.cc without redoing the whole stage2 selection.

#include "ReCalc.hh"

ReCalc::ReCalc(Pipeline& pipe, Site site, Phase phase) :
  Calculator(pipe, site, phase, 0 /* seq */)
{
  TFile* stage2File = pipe.getOutFile();
  reader.init(stage2File, "results");
}

void ReCalc::writeEntries(const char* treename)
{
  TreeWriter<CalcsTree> w(treename);
  w.connect(pipe);

  for (UInt_t entry = 0; entry < reader.size(); ++entry) {
    reader.loadEntry(entry);
    this->seq = reader.data.seq;
    writeEntry(w, reader.data.detector);
  }
}

double ReCalc::livetime_s()
{
  return reader.data.livetime_s;
}

double ReCalc::vetoEff(Det _det, MuonAlg::Purpose purp)
{
  return purp == MuonAlg::Purpose::ForSingles
    ? reader.data.vetoEffSingles
    : reader.data.vetoEff;
}

double ReCalc::nPreMuons(Det _det)
{
  return reader.data.nPreMuons;
}

double ReCalc::nPlusLikeSingles(Det _det)
{
  return reader.data.nPlusLikeSingles;
}

double ReCalc::nPromptLikeSingles(Det _det)
{
  return reader.data.nPromptLikeSingles;
}

double ReCalc::nDelayedLikeSingles(Det _det)
{
  return reader.data.nDelayedLikeSingles;
}
