#pragma once

#include "Readers.cc"

#include "../common/Constants.cc"

#include "../SelectorFramework/core/EventBuf.cc"

class AdBuffer : public EventBuf<AdReader, Det> {
  static constexpr float MIN_AHEAD_US = 2000;

public:
  using Data = algdata_t<AdReader>;

  AdBuffer(Det det) :
    EventBuf(det)
  {
    resize(1000);
  }

  // Det det() { return tag(); };

  bool enough() const final override;
};

bool AdBuffer::enough() const
{
  const float dt = latest().time().diff_us(pending().time());

  return dt > MIN_AHEAD_US;
}
