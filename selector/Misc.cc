#pragma once

#include <vector>
#include <stdexcept>

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

} // namespace util
