#pragma once
#include "EventReader.hh"

Algorithm::Status flasherCut(const EventReader::Data& e);

using FlasherCut = PureAlg<EventReader, flasherCut>;
