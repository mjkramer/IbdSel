#include "MuonSaver.hh"

using Status = Algorithm::Status;

MuonSaver::MuonSaver() :
  outTree("muons")
{
}

void MuonSaver::connect(Pipeline& pipeline)
{
  outTree.connect(pipeline);
  SimpleAlg::connect(pipeline);
}

Status MuonSaver::consume(const EventReader::Data& e)
{
  isMuon_ = false;

  auto put = [&](Float_t strength) {
    isMuon_ = true;

    outTree.data.detector = Det(e.detector);
    outTree.data.trigSec = e.time().s;
    outTree.data.trigNanoSec = e.time().ns;
    outTree.data.strength = strength;

    outTree.fill();
  };

  if (e.isAD() && e.NominalCharge > AD_MIN_CHG)
    put(e.NominalCharge);

  else if (e.isWP() && e.nHit > WP_MIN_NHIT)
    put(e.nHit);

  else if (Det(e.detector) == Det::IWS && e.nHit > IWP_EXTRA_MIN_NHIT)
    put(e.nHit);

  return Status::Continue;
}
