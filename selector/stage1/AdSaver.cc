#include "AdSaver.hh"

#include "EventReader.hh"
#include "MuonSaver.hh"

#include "../common/AdTree.hh"
#include "../common/Constants.hh"
#include "../common/Misc.hh"

#include "../SelectorFramework/core/SimpleAlg.hh"
#include "../SelectorFramework/core/TreeWriter.hh"

AdSaver::AdSaver(Det det) :
  det(det),
  tree(Form("physics_AD%d", int(det))) {}

void AdSaver::connect(Pipeline& pipeline)
{
  tree.connect(pipeline);

  const auto [runNo, fileNo] = runAndFile();
  tree.data.runNo = runNo;
  tree.data.fileNo = fileNo;

  muonSaver = pipeline.getAlg<MuonSaver>();

  SimpleAlg::connect(pipeline);
}

Algorithm::Status AdSaver::consume(const algdata_t<EventReader>& e)
{
  if (not (e.det() != det ||
           e.energy < 0.7 ||
           muonSaver->isMuon())) {
    save(e);
  }

  return Status::Continue;
}

void AdSaver::save(const algdata_t<EventReader>& e)
{
  tree.data.trigSec = e.triggerTimeSec;
  tree.data.trigNanoSec = e.triggerTimeNanoSec;
  tree.data.energy = e.energy;
  tree.data.trigNo = e.triggerNumber;
  tree.data.x = e.x;
  tree.data.y = e.y;
  tree.data.z = e.z;

  tree.fill();
}

std::tuple<UInt_t, UShort_t> AdSaver::runAndFile() const
{
  const char* path = pipe().inFile()->GetName();
  return util::runAndFile(path);
}
