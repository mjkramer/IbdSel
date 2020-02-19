#pragma once

#include "EventReader.hh"

#include "../SelectorFramework/core/SimpleAlg.hh"

Algorithm::Status trigTypeCut(const EventReader::Data& e);

using TrigTypeCut = PureAlg<EventReader, trigTypeCut>;
