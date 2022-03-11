#pragma once

#include "EventReader.hh"

#include "../common/MuonTree.hh"

#include "../SelectorFramework/core/SimpleAlg.hh"
#include "../SelectorFramework/core/TreeWriter.hh"

class MuonSaver : public SimpleAlg<EventReader> {
  // NOTE These are all "exclusive" thresholds, > not >=
  static constexpr int WP_MIN_NHIT = 12;
  static constexpr float AD_MIN_CHG = 3000;
  // NOTE The latest IWP extra veto requires nhit >= 7, but older DocDBs say
  // nhit >= 6, so let's use the latter requirement just to be safe. >5 => >=6
  static constexpr int IWP_EXTRA_MIN_NHIT = 5;

public:
  MuonSaver();
  void connect(Pipeline& pipeline) override;
  Status consume(const EventReader::Data& e) override;
  bool isMuon() const { return isMuon_; }

private:
  TreeWriter<MuonTree> outTree;
  bool isMuon_;
};
