#include "Selectors.hh"
#include "MultCut.hh"
#include "MuonAlg.hh"
#include "VertexCut.hh"

#include "../SelectorFramework/core/Assert.hh"
#include "../SelectorFramework/core/ConfigTool.hh"

#include <TH1F.h>

SelectorBase::SelectorBase(Det det, MuonAlg::Purpose muonAlgPurpose,
                           int multCutTag) :
  det(det),
  muonAlgPurpose(muonAlgPurpose),
  multCutTag(multCutTag),
  BufferedSimpleAlg(TAG(det, multCutTag))
  {}

void SelectorBase::connect(Pipeline &p)
{
  // Without this, we try to find AdBuffer whose tag includes our multCutTag
  this->pred_ = [&](const AdBuffer& reader) { return reader.rawTag() == int(det); };

  BufferedSimpleAlg<AdBuffer>::connect(p);

  muonAlg = p.getAlg<MuonAlg>(muonAlgPurpose);
  ASSERT(muonAlg->rawTag() == int(muonAlgPurpose));

  multCut = p.getTool<MultCutTool>(multCutTag);
  vertexCut = p.getTool<VertexCutTool>();

  const Config* config = p.getTool<Config>();
  initCuts(config);
}

inline
Algorithm::Status SelectorBase::consume_iter(Iter it)
{
  current = it;
  select(it);
  return Status::Continue;
}

// ----------------------------------------------------------------------

SingleSel::SingleSel(Det det, int multCutTag) :
  SelectorBase(det, MuonAlg::Purpose::ForSingles, multCutTag)
{
  auto hname = LeakStr("h_single_AD%d", int(det));
  if (multCutTag == TAG_LOW_DELAYED_EMIN)
    hname = LeakStr("%s_low", hname);

  // Use fine binning since we're doing integrals in Calculator
  // Go up to 20 MeV so we can calc pre-muon rate (for debugging)
  const unsigned nbins = 4000;
  const float emin = 0, emax = 20;

  hist = new TH1F(hname, hname, nbins, emin, emax);

  for (unsigned iR2 = 0; iR2 < R2_BINS; ++iR2) {
    for (unsigned iZ = 0; iZ < Z_BINS; ++iZ) {
      auto hname_px = LeakStr("%s_r2_%d_z_%d", hname, iR2 + 1, iZ + 1);
      hist_pixels[iR2][iZ] = new TH1F(hname_px, hname_px,
                                      nbins, emin, emax);

      for (unsigned iPhi = 0; iPhi < PHI_BINS; ++iPhi) {
        auto hname_px_phi = LeakStr("%s_r2_%d_z_%d_phi_%d", hname,
                                    iR2 + 1, iZ + 1, iPhi);
        hist_pixels_phi[iR2][iZ][iPhi] =
          new TH1F(hname_px_phi, hname_px_phi, nbins, emin, emax);
      }
    }
  }
}

void SingleSel::initCuts(const Config *config)
{
  EMIN = config->get<float>("singleEmin", 0.7);
  // No upper limit (we want premuons):
  EMAX = config->get<float>("singleEmax", 99999);
}

void SingleSel::finalize(Pipeline& _p)
{
  if (multCutTag != 0)
    return;

  hist->Write();

  for (unsigned iR2 = 0; iR2 < R2_BINS; ++iR2)
    for (unsigned iZ = 0; iZ < Z_BINS; ++iZ) {
      hist_pixels[iR2][iZ]->Write();

      for (unsigned iPhi = 0; iPhi < PHI_BINS; ++iPhi)
        hist_pixels_phi[iR2][iZ][iPhi]->Write();
    }
}

void SingleSel::select(Iter it)
{
  if (EMIN < it->energy && it->energy < EMAX &&
      vertexCut->vertexOk(it) &&
      not muonAlg->isVetoed(it->time(), det) &&
      multCut->singleDmcOk(it, det)) {

    hist->Fill(it->energy);

    float x = it->x, y = it->y, z = it->z;

    unsigned iR2 = unsigned((x * x + y * y) / R2_DELTA);
    if (iR2 >= R2_BINS) iR2 = R2_BINS - 1;

    int iZ = int((z - Z_MIN) / Z_DELTA);
    if (iZ < 0) iZ = 0;
    if (iZ >= Z_BINS) iZ = Z_BINS - 1;

    hist_pixels[iR2][iZ]->Fill(it->energy);

    float phi = atan2(y, x);
    float delta_phi = 2 * M_PI / PHI_BINS;
    unsigned iPhi = unsigned((phi + M_PI) / delta_phi);

    hist_pixels_phi[iR2][iZ][iPhi]->Fill(it->energy);
  }
}

// ----------------------------------------------------------------------

IbdSel::IbdSel(Det detector, int tag) :
  SelectorBase(detector, MuonAlg::Purpose::ForIBDs, tag)
{
  if (multCutTag == 0)
    ibdTree = TreeWriter<IbdTree>(LeakStr("ibd_AD%d", int(detector)));
}

void IbdSel::connect(Pipeline& p)
{
  if (multCutTag == 0)
    ibdTree.connect(p);

  SelectorBase::connect(p);
}

void IbdSel::initCuts(const Config *config)
{
  PROMPT_MIN = config->get<float>("ibdPromptEmin", 0.7);
  PROMPT_MAX = config->get<float>("ibdPromptEmax", 12);
  DELAYED_MIN = config->get<float>("ibdDelayedEmin", 6);
  DELAYED_MAX = config->get<float>("ibdDelayedEmax", 12);
  DT_MIN_US = config->get<float>("ibdDtMinUsec", 1);
  DT_MAX_US = config->get<float>("ibdDtMaxUsec", 200);

  auto hname = LeakStr("h_ibd_AD%d", det);
  if (multCutTag == TAG_LOW_DELAYED_EMIN)
    hname = LeakStr("%s_low", hname);

  histP = new TH1F(hname, hname, 40 * PROMPT_MAX, 0, PROMPT_MAX);

  hname = LeakStr("h_ibd_delayed_AD%d", det);
  if (multCutTag == TAG_LOW_DELAYED_EMIN)
    hname = LeakStr("%s_low", hname);

  histD = new TH1F(hname, hname, 40 * DELAYED_MAX, 0, DELAYED_MAX);
}

void IbdSel::finalize(Pipeline& p)
{
  histP->Write();
  histD->Write();
}

void IbdSel::save(Iter prompt, Iter delayed)
{
  if (multCutTag != 0)
    return;

  ibdTree.data.runNo = delayed->runNo;
  ibdTree.data.fileNo = delayed->fileNo;
  ibdTree.data.trigP = prompt->trigNo;
  ibdTree.data.trigD = delayed->trigNo;
  ibdTree.data.eP = prompt->energy;
  ibdTree.data.eD = delayed->energy;
  ibdTree.data.dt_us = delayed->time().diff_us(prompt->time());
  ibdTree.data.xP = prompt->x;
  ibdTree.data.yP = prompt->y;
  ibdTree.data.zP = prompt->z;
  ibdTree.data.xD = delayed->x;
  ibdTree.data.yD = delayed->y;
  ibdTree.data.zD = delayed->z;

  ibdTree.fill();
}

void IbdSel::select(Iter delayed)
{
  auto dt_us = [&](Iter other) { return delayed->time().diff_us(other->time()); };

  if (DELAYED_MIN < delayed->energy && delayed->energy < DELAYED_MAX &&
      vertexCut->vertexOk(delayed) &&
      not muonAlg->isVetoed(delayed->time(), det)) {

    for (Iter prompt = delayed.earlier();
         dt_us(prompt) < DT_MAX_US;
         prompt = prompt.earlier()) {

      if (PROMPT_MIN < prompt->energy && prompt->energy < PROMPT_MAX &&
          dt_us(prompt) > DT_MIN_US &&
          vertexCut->vertexOk(prompt) &&
          multCut->ibdDmcOk(prompt, delayed, det)) {

        save(prompt, delayed);

        histP->Fill(prompt->energy);
        histD->Fill(delayed->energy);
      }
    }
  }
}
