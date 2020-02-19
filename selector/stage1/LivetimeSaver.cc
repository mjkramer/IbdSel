#include "LivetimeSaver.hh"

Status LivetimeSaver::consume(const EventReader::Data& e)
{
  last_integralRunTime_ms = e.integralRunTime_ms;

  return Status::Continue;
}

void LivetimeSaver::finalize(Pipeline& _pipeline)
{
  auto h = new TH1F(keys::HistLivetime, "Livetime (s)", 1, 0, 1);
  h->SetBinContent(1, last_integralRunTime_ms / 1000);
  h->Write();
}
