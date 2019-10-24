#pragma once
#include "EventReader.hh"

Algorithm::Status flasherCut(const EventReader::Data& e);

#ifndef __CLING__
using FlasherCut = PureAlg<EventReader, flasherCut>;
#endif
