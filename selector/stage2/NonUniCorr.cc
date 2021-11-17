#include "NonUniCorr.hh"
#include "stage2_globals.hh"

#include "../common/AdTree.hh"
#include "../common/Misc.hh"

#include "../SelectorFramework/core/ConfigTool.hh"

#include <TH2F.h>
#include <TFile.h>

static std::string dataPath()
{
  std::string base = IBDSEL_BASE;
  return base + "/static/nonuni/non_uniformity_correction_maps.root";
}

void NonUniCorrTool::connect(Pipeline& p)
{
  auto config = p.getTool<Config>();
  bool useAlphasOnly = config->get<bool>("newNonUniAlphasOnly");
  const char *desc = useAlphasOnly ? "alpha_only" : "alpha_ngd";

  auto path = dataPath();
  TFile infile(path.c_str());

  auto getMap = [&](Det det, const char* periodName) {
    auto name = Form("h_nu_corr_map_%s_r2z_eh%d_ad%d_%s",
                     desc, stage2::site, det, periodName);
    auto h = (TH2F*) infile.Get(name);
    h->SetDirectory(nullptr);   // don't go away!
    return h;
  };

  for (Det det : util::ADsFor(stage2::site, stage2::phase)) {
    auto& hs = h_correction_maps;
    auto idet = util::idet(det);
    hs[idet][0] = getMap(det, "before20170331");
    hs[idet][1] = getMap(det, "after20170331");
  }
}

size_t NonUniCorrTool::getPeriod(UInt_t timeSec)
{
  for (size_t p = 0; p < N_PERIODS - 1; ++p) {
    if (timeSec <= periodDivTimeSec[p])
      return p;
  }

  return N_PERIODS - 1;
}

float NonUniCorrTool::correctedEnergy(Det det, AdTree& data)
{
  float x = data.x, y = data.y, z = data.z;

  UInt_t r2_bin = UInt_t((x*x + y*y) / 0.4e6) + 1;
  if (r2_bin > 10) r2_bin = 10;

  UInt_t z_bin = UInt_t((z + 2000) / 400) + 1;
  if (z_bin < 1) z_bin = 1;
  if (z_bin > 10) z_bin = 10;

  size_t period = getPeriod(data.trigSec);
  TH2F* map = h_correction_maps[util::idet(det)][period];
  float corr = map->GetBinContent(r2_bin, z_bin);
  return corr * data.energy;
}
