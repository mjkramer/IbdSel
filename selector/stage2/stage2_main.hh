#pragma once

#include "../common/Constants.hh"

// For evaluating efficiency of delayed energy cut
const int TAG_LOW_DELAYED_EMIN = 1;
const float LOW_DELAYED_EMIN = 4;

void stage2_main(const char* confFile, const char* inFile, const char* outFile,
                 Site site, Phase phase, UInt_t seq);
