#pragma once
#include "EventReader.hh"

Algorithm::Status trigTypeCut(const EventReader::Data& e);

#ifndef __CLING__
using TrigTypeCut = PureAlg<EventReader, trigTypeCut>;
#endif
