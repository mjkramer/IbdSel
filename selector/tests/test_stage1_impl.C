void test_stage1_impl(int runno, int fileno, int site, int phase)
{
  gErrorIgnoreLevel = kError;   // suppress dictionary warnings

  std::string f_in = FileFinder().find(runno, fileno);
  auto f_out = "tests/out_stage1.root";

  stage1_main(f_in.c_str(), f_out, Site(site), Phase(phase));
}
