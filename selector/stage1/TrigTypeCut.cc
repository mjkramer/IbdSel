#include "TrigTypeCut.hh"

Algorithm::Status trigTypeCut(const EventReader::Data& e)
{
  std::initializer_list<unsigned int> badTrigTypes = {
    0x10000001,    // manual
    0x10000002,    // cross
    0x10000004,    // periodic
    0x10000008,    // pedestal
    0x10000010,    // calib
    0x10000020,    // random
  };

  for (const auto mask : badTrigTypes) {
    if ((e.triggerType & mask) == mask)
      return Algorithm::Status::SkipToNext;
  }

  // NOTE SYSU requires a mult trigger for WP muons, so let's do the same
  // (especially since we're using their muon-decay bkg rates). As of
  // end-of-data-taking, the hardware NHIT threshold for the IWP is 6, while for
  // the OWP it is 7 (8) near (far). So this cut should have minimal effect on
  // regular (n>12) WP muons, but it could make a difference for the extra 7-12
  // hit IWP veto (for cases where the hardware and software NHIT differ).
  const unsigned int kMult = 0x10000100;
  if (e.isWP() && not ((e.triggerType & kMult) == kMult))
    return Algorithm::Status::SkipToNext;

  return Algorithm::Status::Continue;
}
