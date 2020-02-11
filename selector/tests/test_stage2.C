void test_stage2(int site, int phase)
{
  gROOT->ProcessLine(".x cling/Build.C(\"stage2/stage2_main.cc\")");

  gROOT->ProcessLine(Form(".x tests/test_stage2_impl.C(%d, %d)", site, phase));
}
