void recalc(const char* confFile, const char* stage2File, Site site, Phase phase)
{
  Pipeline p;
  p.makeOutFile(stage2File, Pipeline::DefaultFile, false /* reopen */, "UPDATE");
  p.makeTool<Config>(confFile);
  p.process({});                // just to initialize Config

}

int main(int argc, char** argv)
{
  return 0;
}
