{
  auto f_in = "/global/projecta/projectdirs/dayabay/data/dropbox/p17b/lz3.skim.6/recon.Neutrino.0021221.Physics.EH1-Merged.P17B-P._0001.root";
  auto f_out = "tests/out_stage1.root";
  auto stage = Stage::k6AD;
  auto site = Site::EH1;

  stage1_main(f_in, f_out, stage, site);
}
