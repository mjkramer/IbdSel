#include "stage2_main.hh"

#include <iostream>
#include <string>

int main(int argc, char** argv)
{
  if (argc != 6 + 1) {
    std::cerr << "Usage: " << argv[0] << "conffile infile outfile site phase seq";
    return 1;
  }

  const auto confFile = argv[1];
  const auto inFile = argv[2];
  const auto outFile = argv[3];
  const auto site = Site(std::stoul(argv[4]));
  const auto phase = Phase(std::stoul(argv[5]));
  const auto seq = std::stoul(argv[6]);

  stage2_main(confFile, inFile, outFile, site, phase, seq);

  return 0;
}
