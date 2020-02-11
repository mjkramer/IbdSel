#pragma once

#include "Constants.cc"

#include <boost/algorithm/string.hpp>

#include <stdexcept>
#include <vector>

namespace util {

std::vector<Det> ADsFor(Site site, Phase phase)
{
  if (site == Site::EH1) {
    if (phase == Phase::k7AD)
      return { Det::AD2 };
    else
      return { Det::AD1, Det::AD2 };
  }

  if (site == Site::EH2) {
    if (phase == Phase::k6AD)
      return { Det::AD1 };
    else
      return { Det::AD1, Det::AD2 };
  }

  if (site == Site::EH3) {
    if (phase == Phase::k6AD)
      return { Det::AD1, Det::AD2, Det::AD3 };
    else
      return { Det::AD1, Det::AD2, Det::AD3, Det::AD4 };
  }

  throw std::runtime_error("Invalid site/phase in ADsFor");
}

std::tuple<UInt_t, UShort_t> runAndFile(const std::string& path)
{
  namespace A = boost::algorithm;

  auto base = path.substr(path.find_last_of("/") + 1);
  std::vector<std::string> parts;
  A::split(parts, base, A::is_any_of("."));

  return { atoi(parts[2].c_str()), atoi(&parts[6].c_str()[1]) };
}

} // namespace util
