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

get_property(FROM_SELECTOR TARGET SelectorFramework PROPERTY SELECTOR_HEADERS)
get_property(SELECTOR_INCDIR TARGET SelectorFramework PROPERTY INCLUDE_DIRECTORIES)

set(STAGE1_HEADERS "${FROM_SELECTOR}" "${STAGE1_HEADERS}" )
message("${STAGE1_HEADERS}")

include_directories(${SELECTOR_INCDIR})

target_link_libraries(stage1 PUBLIC SelectorFramework)
target_include_directories(stage1 PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_include_directories(stage1 PUBLIC "${SELECTOR_INCDIR}")
target_link_libraries(stage1 PUBLIC common)

ROOT_GENERATE_DICTIONARY(Dict_stage1 "${STAGE1_HEADERS}" MODULE stage1 OPTIONS -interpreteronly)
target_link_libraries(stage1.exe PUBLIC stage1)
