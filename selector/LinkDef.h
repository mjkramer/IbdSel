#ifdef __CLING__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ function DataTypeToChar;

#pragma link C++ class Pipeline-;
#pragma link C++ class Tool-;
#pragma link C++ class EventReader-;

#pragma link C++ function trigTypeCut;

#pragma link C++ class ClusterAlg-;
#pragma link C++ class ClusterSaver-;
#pragma link C++ nestedclass EventReader-;
#pragma link C++ class MuonAlg-;
#pragma link C++ class MuonSaver-;
#pragma link C++ class LivetimeSaver-;
#pragma link C++ class SelectIBD-;
#pragma link C++ class SelectSingles-;

#pragma link C++ function makeAlg<ClusterAlg>;
#pragma link C++ function makeAlg<SelectIBD>;
#pragma link C++ function makeAlg<SelectSingles>;

#pragma link C++ function stage1_main;
#pragma link C++ function ibd_main;

#endif
