#include "ReCalc.hh"

#include <iostream>
#include <string>

void recalc(const char* confFile, const char* stage2File, Site site, Phase phase)
{
  Pipeline p;
  p.makeOutFile(stage2File, Pipeline::DefaultFile, false /* reopen */, "UPDATE");
  p.makeTool<Config>(confFile);
  p.process({});                // just to initialize Config

  ReCalc recalc(p, site, phase);
  recalc.writeEntries();
}

int main(int argc, char** argv)
{
  if (argc != 4 + 1) {
    std::cerr << "Usage: " << argv[0] << "confFile stage2File site phase";
    return 1;
  }

  const auto confFile = argv[1];
  const auto stage2File = argv[2];
  const auto site = Site(std::stoul(argv[3]));
  const auto phase = Phase(std::stoul(argv[4]));

  recalc(confFile, stage2File, site, phase);

  return 0;
}
