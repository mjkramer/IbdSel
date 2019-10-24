#pragma once

#include "EventReader.hh"
#include "MuonSaver.hh"
#include "ClusterTree.hh"

#include "TreeWriter.hh"

using Status = Algorithm::Status;

class ClusterSaver : public SimpleAlg<EventReader> {
  static constexpr float GAPSIZE_US = 1000;
public:
  ClusterSaver(int detector);
  void connect(Pipeline& pipeline) override;
  Status consume(const EventReader::Data& e) override;

  const short detector;

private:
  void save();

  TreeWriter<ClusterTree> singlesTree, clustersTree;
  std::vector<EventReader::Data> events;

  const MuonSaver* muonSaver;
};

inline ClusterSaver::ClusterSaver(int detector) :
  detector(detector),
  singlesTree(Form("singles_AD%d", detector)),
  clustersTree(Form("clusters_AD%d", detector))
{
}

inline void ClusterSaver::connect(Pipeline& pipeline)
{
  singlesTree.connect(pipeline);
  clustersTree.connect(pipeline);

  muonSaver = pipeline.getAlg<MuonSaver>();

  SimpleAlg::connect(pipeline);
}

inline Status ClusterSaver::consume(const EventReader::Data& e)
{
  if (e.detector != detector ||
      e.energy < 0.7 ||
      muonSaver->isMuon())
    return Status::Continue;

  if (events.size() == 0) {     // Very first event
    events.push_back(e);
    return Status::Continue;
  }

  const Time t_this = e.time();
  const Time t_last = events.back().time();

  if (t_this.diff_us(t_last) > GAPSIZE_US) {
    save();
    events.clear();
  }

  events.push_back(e);

  return Status::Continue;
}

inline void ClusterSaver::save()
{
  const int N = events.size();
  TreeWriter<ClusterTree>& outTree = N > 1 ? clustersTree : singlesTree;
  outTree.data.size = N;

  for (int i = 0; i < N; ++i) {
    const auto& e = events[i];
    outTree.data.trigSec[i] = e.triggerTimeSec;
    outTree.data.trigNanoSec[i] = e.triggerTimeNanoSec;
    outTree.data.energy[i] = e.energy;
  }

  outTree.fill();
}
