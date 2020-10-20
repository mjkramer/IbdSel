set(STAGE1_HEADERS 
    AdSaver.hh
    EventReader.hh
    FlasherCut.hh
    LivetimeSaver.hh
    MuonSaver.hh
    stage1_main.hh
    TrigTypeCut.hh
    )

add_library(stage1 SHARED
    stage1_main.cc
    AdSaver.cc
    EventReader.cc
    FlasherCut.cc
    LivetimeSaver.cc
    MuonSaver.cc
    TrigTypeCut.cc
)

add_executable(stage1.exe 
    stage1_exe.cpp
)

ROOT_GENERATE_DICTIONARY(Dict_stage1 "${STAGE1_HEADERS}" MODULE stage1)
target_link_libraries(stage1 PUBLIC common)
target_include_directories(stage1 PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(stage1.exe PUBLIC stage1)
