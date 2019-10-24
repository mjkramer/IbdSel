#include "TrigTypeCut.hh"

Algorithm::Status trigTypeCut(const EventReader::Data& e)
{
  const UInt_t crossMask = 0x10000002;
  bool cross = (e.triggerType & crossMask) == crossMask;

  return vetoIf(cross);
}

//template class PureAlg<EventReader, trigTypeCut>;
