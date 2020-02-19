#pragma once

#include "EventReader.hh"

#include "../SelectorFramework/core/SimpleAlg.hh"

Algorithm::Status flasherCut(const EventReader::Data& e);

using FlasherCut = PureAlg<EventReader, flasherCut>;
