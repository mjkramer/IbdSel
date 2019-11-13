void test_stage2(int stage, int site)
{
  const char* debug = getenv("IBDSEL_DEBUG") ? "g" : "";
  gROOT->ProcessLine(".x LoadBoost.C");
  gROOT->ProcessLine(Form(".L stage2_main.cc+%s", debug));
  gROOT->ProcessLine(Form(".x tests/test_stage2_impl.C(%d, %d)", stage, site));
}
