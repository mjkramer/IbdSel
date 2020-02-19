#pragma once

#include "EventReader.hh"

#include "../common/AdTree.hh"

#include "../SelectorFramework/core/SimpleAlg.hh"
#include "../SelectorFramework/core/TreeWriter.hh"

class MuonSaver;

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
