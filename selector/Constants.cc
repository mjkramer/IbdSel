#pragma once

enum class Stage : UChar_t { k6AD = 1, k8AD, k7AD };

enum class Site : UChar_t { EH1 = 1, EH2, EH3 };

enum class Det : UChar_t { AD1 = 1, AD2, AD3, AD4 };

namespace keys {
  const char* HistLivetime = "h_livetime";
}

namespace units {
  constexpr float hzToDaily = 86'400;
}
