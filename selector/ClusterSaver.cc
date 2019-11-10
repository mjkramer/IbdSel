#pragma once

#include "EventReader.cc"
#include "MuonSaver.cc"
#include "ClusterTree.cc"
#include "Constants.cc"

#include "SelectorFramework/core/TreeWriter.cc"

#include <tuple>
#include <boost/algorithm/string.hpp>
#include <cstdlib>

using Status = Algorithm::Status;

class ClusterSaver : public SimpleAlg<EventReader> {
public:
  static constexpr float GAPSIZE_US = 1000;

  ClusterSaver(Det detector);
  void connect(Pipeline& pipeline) override;
  Status consume(const EventReader::Data& e) override;

  const Det detector;

private:
  void save();

  std::tuple<UInt_t, UShort_t> runAndFile() const;

  TreeWriter<ClusterTree> singlesTree, clustersTree;
  std::vector<EventReader::Data> events;

  const MuonSaver* muonSaver;
};

ClusterSaver::ClusterSaver(Det detector) :
  detector(detector),
  singlesTree(Form("singles_AD%d", int(detector))),
  clustersTree(Form("clusters_AD%d", int(detector)))
{
}

void ClusterSaver::connect(Pipeline& pipeline)
{
  const auto [runNo, fileNo] = runAndFile();

  for (auto tree : { &singlesTree, &clustersTree }) {
    tree->connect(pipeline);

    tree->data.runNo = runNo;
    tree->data.fileNo = fileNo;
  }

  muonSaver = pipeline.getAlg<MuonSaver>();

  SimpleAlg::connect(pipeline);
}

Status ClusterSaver::consume(const EventReader::Data& e)
{
  if (e.det() != detector ||
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

void ClusterSaver::save()
{
  const int N = events.size();
  TreeWriter<ClusterTree>& outTree = N > 1 ? clustersTree : singlesTree;
  outTree.data.size = N;

  for (int i = 0; i < N; ++i) {
    const auto& e = events[i];
    outTree.data.trigSec[i] = e.triggerTimeSec;
    outTree.data.trigNanoSec[i] = e.triggerTimeNanoSec;
    outTree.data.energy[i] = e.energy;

    outTree.data.trigNo[i] = e.triggerNumber;
  }

  outTree.fill();
}

std::tuple<UInt_t, UShort_t> runAndFile(const std::string& path)
{
  namespace A = boost::algorithm;

  auto base = path.substr(path.find_last_of("/") + 1);
  std::vector<std::string> parts;
  A::split(parts, base, A::is_any_of("."));

  return { atoi(parts[2].c_str()), atoi(&parts[6].c_str()[1]) };
}

std::tuple<UInt_t, UShort_t> ClusterSaver::runAndFile() const
{
  const char* path = pipe().inFile()->GetName();
  return ::runAndFile(path);
}
