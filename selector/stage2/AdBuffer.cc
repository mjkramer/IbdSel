#include "AdBuffer.hh"

AdBuffer::AdBuffer(Det det) :
  EventBuf(det)
{
  resize(1000);
}

bool AdBuffer::enough() const
{
  const float dt = latest().time().diff_us(pending().time());

  return dt > MIN_AHEAD_US;
}
