#pragma once

#include "../common/Constants.hh"

#include "../SelectorFramework/core/Kernel.hh"

class AdTree;

class TH2F;

class NonUniCorrTool : public Tool {
public:
  void connect(Pipeline& pipeline) override;
  float correctedEnergy(Det det, AdTree& data);

private:
  size_t getPeriod(UInt_t timeSec);

  static constexpr size_t N_PERIODS = 2;

  TH2F* h_correction_maps[4][N_PERIODS];

  UInt_t periodDivTimeSec[N_PERIODS] = {1490911200};
};
