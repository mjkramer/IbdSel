void test_stage2(int stage, int site)
{
  gROOT->ProcessLine(".L stage2_main.cc+g");
  gROOT->ProcessLine(Form(".x tests/test_stage2_impl.C(%d, %d)", stage, site));
}
