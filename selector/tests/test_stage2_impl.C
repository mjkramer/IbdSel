void test_stage2_impl(int stage, int site)
{
  auto f_conf = "tests/test.cfg";
  auto f_in = "tests/out_stage1.root";
  auto f_out = "tests/out_stage2.root";
  auto seq = 0u;

  stage2_main(f_conf, f_in, f_out, Stage(stage), seq, Site(site));
}
