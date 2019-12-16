#pragma once

#include <TROOT.h>

enum class Stage : UChar_t { k6AD = 1, k8AD, k7AD };

enum class Site : UChar_t { EH1 = 1, EH2, EH3 };

enum class Det : UChar_t { AD1 = 1, AD2, AD3, AD4, IWS, OWS };

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
