#include "Misc.hh"

#include <boost/algorithm/string.hpp>

#include <algorithm>
#include <stdexcept>

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

std::vector<size_t> iADsFor(Site site, Phase phase)
{
  const auto ADs = ADsFor(site, phase);
  std::vector<size_t> result;
  std::transform(ADs.begin(), ADs.end(), result.begin(),
                 [](Det d) { return size_t(d) - 1;});
  return result;
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
