#pragma once

#include <vector>
#include <stdexcept>
#include <boost/algorithm/string.hpp>

#include "Constants.cc"

namespace util {

std::vector<Det> ADsFor(Site site, Stage stage)
{
  if (site == Site::EH1) {
    if (stage == Stage::k7AD)
      return { Det::AD2 };
    else
      return { Det::AD1, Det::AD2 };
  }

  if (site == Site::EH2) {
    if (stage == Stage::k6AD)
      return { Det::AD1 };
    else
      return { Det::AD1, Det::AD2 };
  }

  if (site == Site::EH3) {
    if (stage == Stage::k6AD)
      return { Det::AD1, Det::AD2, Det::AD3 };
    else
      return { Det::AD1, Det::AD2, Det::AD3, Det::AD4 };
  }

  throw std::runtime_error("Invalid site/stage in ADsFor");
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
