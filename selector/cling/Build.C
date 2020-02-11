void Build(const char* srcfile)
{
  const char* debug = getenv("IBDSEL_DEBUG") ? "g" : "";

  gROOT->ProcessLine(".x cling/LoadBoost.C");
  gROOT->ProcessLine(Form(".L %s+%s", srcfile, debug));
}
