#pragma once
#include "EventReader.cc"

Algorithm::Status trigTypeCut(const EventReader::Data& e);

using TrigTypeCut = PureAlg<EventReader, trigTypeCut>;
