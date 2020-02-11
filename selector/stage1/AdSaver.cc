#pragma once

#include "EventReader.cc"
#include "MuonSaver.cc"

#include "../common/AdTree.cc"
#include "../common/Constants.cc"
#include "../common/Misc.cc"

#include "../SelectorFramework/core/TreeWriter.cc"

class AdSaver : public SimpleAlg<EventReader> {
public:
  AdSaver(Det det);
  void connect(Pipeline& pipeline) override;
  Algorithm::Status consume(const algdata_t<EventReader>& e) override;

  const Det det;

private:
  void save(const algdata_t<EventReader>& e);
  std::tuple<UInt_t, UShort_t> runAndFile() const;

  TreeWriter<AdTree> tree;

  const MuonSaver* muonSaver;
};

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

  tree.fill();
}

std::tuple<UInt_t, UShort_t> AdSaver::runAndFile() const
{
  const char* path = pipe().inFile()->GetName();
  return util::runAndFile(path);
}
