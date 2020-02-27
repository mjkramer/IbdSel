#pragma once

#include "Constants.hh"

#include <string>
#include <tuple>
#include <vector>

namespace util {

std::vector<Det> ADsFor(Site site, Phase phase);
std::vector<size_t> iADsFor(Site site, Phase phase);

std::tuple<UInt_t, UShort_t> runAndFile(const std::string& path);

} // namespace util
