#pragma once
#include "EventReader.cc"

Algorithm::Status flasherCut(const EventReader::Data& e);

using FlasherCut = PureAlg<EventReader, flasherCut>;
