void test_stage2_impl(int site, int phase)
{
  auto f_conf = "../static/configs/config.nominal.txt";
  auto f_in = "tests/out_stage1.root";
  auto f_out = "tests/out_stage2.root";
  auto seq = 0u;

  stage2_main(f_conf, f_in, f_out, Site(site), Phase(phase), seq);
}
