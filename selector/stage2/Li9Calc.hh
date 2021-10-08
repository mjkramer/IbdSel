#pragma once

#include "../common/Constants.hh"

#include <functional>
#include <map>
#include <string>

class Li9Calc {
public:
  enum class Mode { Nominal, NoB12, Fix15pctHe8, NoHe8 };

  Li9Calc();

  // This return the rate (per AD per day) predicted in our raw spectrum
  // assuming perfect veto/dmc efficiencies
  double li9daily_nearest(Site site, double shower_pe, double showerVeto_ms);
  double li9daily_linreg(Site site, double shower_pe, double showerVeto_ms);

  void setMode(Mode mode);

private:
  static constexpr const char* DATA_DIR = "static/li9_data";
  static constexpr const char* LI9_FILENAME = "new_Li9_rates.txt";

  static constexpr unsigned MIN_PE = 5'000;
  static constexpr unsigned MAX_NTAG_PE = 160'000;

  // for linear regression:
  static constexpr unsigned MIN_VETO_BDRY_PE = 180'000; // inclusive
  static constexpr unsigned MAX_VETO_BDRY_PE = 500'000; // inclusive
  static constexpr unsigned DELTA_VETO_BDRY_PE = 10'000;

  static constexpr unsigned MAX_PE = 9'999'999;

  static constexpr double LI9_LIFETIME_MS = 257;
  static constexpr double HE8_LIFETIME_MS = 171.6;

  // From Chris's technote:

  static constexpr double LI9_SEL_SURV_PROB = 0.21; // exp(-400/257)

  // XXX where did 0.98 come from? I can't find it. We should switch to unity
  // since that's what RunBkgAna seems to use?
  static constexpr double NTAG_EFF = 1; // 45% uncertainty

  static constexpr double NEAR_LOW_PE_PROMPT_EFF = 0.168; // 8 MeV
  static constexpr double FAR_LOW_PE_PROMPT_EFF = 0.43; // 6 MeV
  static constexpr double HIGH_PE_PROMPT_EFF = 0.783; // 3.5 MeV

  static constexpr double LIVEDAYS[3] = {1737, 1729, 1737};
  static constexpr double NDET_WEIGHTED[3] = {1.8888, 1.8925, 3.8921};
  // XXX we should be using the veto efficiencies *without* the shower veto!
  // static constexpr double VETO_EFFS[3] = {0.82, 0.85, 0.98};
  // These are the efficiencies w/o shower veto (EHx-AD1 8AD, muveto_toy):
  static constexpr double VETO_EFFS[3] = {0.8711, 0.9015, 0.9883};

  // static constexpr double MULT_EFF = 0.97;
  static constexpr double MULT_EFFS[3] = {0.9772, 0.9782, 0.9829};
  // static constexpr double DT_EFF = 0.96;

  static constexpr double FRAC_HE8_NOM = 0.055;

  // These structs should be private, but then the _dict.cxx generated by
  // rootcling will still try to access them (because we enabled the
  // nestedclasses pragma?), causing a compiler error. Maybe we should report
  // this as a ROOT bug. (Why doesn't ACLiC fail?)
public:
  struct Data {
    double nomRate, statUnc, chi2, rateNoB12, rate15pctHe8, rateNoHe8;
  };

  struct MuonBin {
    unsigned lowerPE, upperPE;

    bool operator<(const MuonBin& rhs) const // needed for std::map
    {
      return lowerPE < rhs.lowerPE ||
        (lowerPE == rhs.lowerPE && upperPE < rhs.upperPE);
    }
  };

private:
  double get(Site site, MuonBin bin);

  using SiteTable = std::map<MuonBin, Data>;
  using Table = std::map<Site, SiteTable>;
  using LivetimeTable = std::map<Site, double>;

  std::string dataPath(const char* filename);
  void initTable();
  double frac_he8();
  double measLowRange(Site site);
  double measMidRange(Site site, unsigned shower_pe);
  double measHighRange(Site site, unsigned shower_pe);
  double interpolate(unsigned shower_pe,
                     std::function<double(unsigned)> getter);

  Table table;
  LivetimeTable adLivedays;
  Mode mode;
};

