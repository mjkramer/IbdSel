#pragma once

#include "Readers.hh"
#include "stage2_globals.hh"

#include "../common/Misc.hh"

#include "../SelectorFramework/core/Kernel.hh"
#include "../SelectorFramework/core/RingBuf.hh"
#include "../SelectorFramework/core/Util.hh"

#include <algorithm>

class GapFinder : public Algorithm {
  static constexpr int BUFSIZE = 10000;
  static constexpr float GAP_THRESH_US = 10e6;

public:
  GapFinder()
    : GapFinder(std::make_index_sequence<4>{}) {}

  template <size_t... Is>
  GapFinder(std::index_sequence<Is...>) :
    iADs(util::iADsFor(stage2::site, stage2::phase)),
    adBufs{(void(Is), BUFSIZE)...} {}

  void connect(Pipeline&) override;
  Status execute() override;

  struct PendingGap {
    Time t1, t2;
    bool needToCheck[4] = {0};
  };

private:
  void publish(PendingGap&);

  const std::vector<size_t> iADs;
  const AdReader* adReaders[4] = {0};
  RingBuf<Time> adBufs[4];
  std::vector<PendingGap> pending;
};

void GapFinder::connect(Pipeline& p)
{
  for (const auto idet: iADs) {
    const Det det = Det(idet + 1);
    adReaders[idet] = p.getAlg<AdReader>(det);
  }
}

Algorithm::Status GapFinder::execute()
{
  for (const auto idet: iADs) {
    if (adReaders[idet]->ready()) {
      const Time t = adReaders[idet]->data.time();

      bool refinedPendingGap = false;

      for (auto it = pending.begin(); it != pending.end(); ++it) {
        PendingGap& gap = *it;

        if (not gap.needToCheck[idet])
          continue;

        if (t < gap.t2)
          continue;

        gap.needToCheck[idet] = false;
        refinedPendingGap = true;

        const Time midPt = gap.t1.shifted_us(0.5 * gap.t2.diff_us(gap.t1));

        for (const Time tt : adBufs[idet]) {
          if (tt < gap.t1)
            break;

          if (tt < gap.t2) {
            if (tt > midPt)
              gap.t2 = tt;
            else
              gap.t1 = tt;
          }
        }

        if (not std::any_of(gap.needToCheck, gap.needToCheck+4,
                            [](bool b) { return b; })) {
          publish(gap);
          it = pending.erase(it) - 1; // subtract 1 due to increment in "for" stmt
        }
      } // loop over pending

      if (not refinedPendingGap) {
        if (t.diff_us(adBufs[idet].top()) > GAP_THRESH_US) {
          PendingGap gap;
          gap.t1 = adBufs[idet].top();
          gap.t2 = t;
          for (const auto idet2 : iADs) {
            if (idet2 != idet)
              gap.needToCheck[idet] = true;
          }
          pending.push_back(gap);
        }
      }

      adBufs[idet].put(t);
    } // ready?
  }   // idet
  return Status::Continue;
}

void GapFinder::publish(PendingGap& gap)
{
}
