#pragma once
#include "EventReader.hh"

Algorithm::Status trigTypeCut(const EventReader::Data& e);

using TrigTypeCut = PureAlg<EventReader, trigTypeCut>;
