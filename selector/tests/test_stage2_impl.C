{
  auto f_conf = "tests/test.cfg";
  auto f_in = "tests/out_stage1.root";
  auto f_out = "tests/out_stage2.root";
  auto stage = Stage::k6AD;
  auto seq = 0u;
  auto site = Site::EH1;

  stage2_main(f_conf, f_in, f_out, stage, seq, site);
}
