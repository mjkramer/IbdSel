#pragma once

#include "Readers.hh"

#include "Constants.hh"

#include "EventBuf.hh"

class AdBuffer : public EventBuf<AdReader, Det> {
  static constexpr float MIN_AHEAD_US = 2000;

public:
  using Data = algdata_t<AdReader>;

  AdBuffer(Det det);

  // Det det() { return tag(); };

  bool enough() const final override;
};
