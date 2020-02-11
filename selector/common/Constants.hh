#pragma once

#include <TROOT.h>

// These used to be ": UChar_t", but PyROOT then chokes when dealing with
// vectors of them, assuming they are the default size.

enum class Phase { k6AD = 1, k8AD, k7AD };

enum class Site { EH1 = 1, EH2, EH3 };

enum class Det { AD1 = 1, AD2, AD3, AD4, IWS, OWS };

size_t idx_of(Det d)
{
  return size_t(d) - 1;
}

namespace keys {
  const char* HistLivetime = "h_livetime";
}

namespace units {
  constexpr float hzToDaily = 86'400;
}
