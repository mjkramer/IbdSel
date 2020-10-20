cmake_minimum_required(VERSION 3.12)
project(IbdSel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

find_package(Boost REQUIRED)
if(Boost_FOUND)
    message(STATUS "found Boost version ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION} in ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost not found")
endif()

find_package(ROOT CONFIG REQUIRED)
include(${ROOT_USE_FILE})

# fill the macro necessary for Li9 calculations
execute_process(
    COMMAND bash -c "echo $(dirname $(dirname $PWD))"
    OUTPUT_VARIABLE IBDSEL_BASE
    )
string(STRIP ${IBDSEL_BASE} IBDSEL_BASE)

include_directories(
                    common
                    stage1
                    stage2
                    stage2/external
                    SelectorFramework/core
                    )

set(SELECTOR_FRAMEWORK_HEADERS
    Assert.hh
    BaseIO.hh
    Clock.hh
    ConfigTool.hh
    DirectReader.hh
    EventBuf.hh
    Kernel.hh
    PrefetchLooper.hh
    RingBuf.hh
    SimpleAlg.hh
    SyncReader.hh
    TimeSyncReader.hh
    TreeWriter.hh
    Util.hh
    )
add_library(SelectorFramework STATIC
    SelectorFramework/core/BaseIO.cc
    SelectorFramework/core/Clock.cc
    SelectorFramework/core/ConfigTool.cc
    SelectorFramework/core/Kernel.cc
    SelectorFramework/core/Util.cc
    )
ROOT_GENERATE_DICTIONARY(Dict_SelectorFramework "${SELECTOR_FRAMEWORK_HEADERS}" 
                         MODULE SelectorFramework
                         )
target_link_libraries(SelectorFramework PUBLIC ROOT::Core ROOT::Hist ROOT::Tree)

set(COMMON_HEADERS 
    AdTree.hh
    Constants.hh
    Misc.hh
    MuonTree.hh
    )
add_library(common  STATIC common/AdTree.cc common/Misc.cc common/MuonTree.cc)
ROOT_GENERATE_DICTIONARY(Dict_common "${COMMON_HEADERS}" MODULE common)
target_link_libraries(common PUBLIC SelectorFramework)

set(STAGE1_HEADERS 
    AdSaver.hh
    EventReader.hh
    FlasherCut.hh
    LivetimeSaver.hh
    MuonSaver.hh
    stage1_main.hh
    TrigTypeCut.hh
    )
add_executable(stage1.exe 
    stage1/stage1_exe.cpp
    stage1/stage1_main.cc
    stage1/AdSaver.cc
    stage1/EventReader.cc
    stage1/FlasherCut.cc
    stage1/LivetimeSaver.cc
    stage1/MuonSaver.cc
    stage1/TrigTypeCut.cc
)
ROOT_GENERATE_DICTIONARY(Dict_stage1 "${STAGE1_HEADERS}" MODULE stage1.exe)
target_link_libraries(stage1.exe PUBLIC common)

set(STAGE2_HEADERS
    AdBuffer.hh
    CalcsTree.hh
    Calculator.hh
    GapFinder.hh
    IbdTree.hh
    Li9Calc.hh
    MultCut.hh
    MuonAlg.hh
    Readers.hh
    ReCalc.hh
    Selectors.hh
    stage2_globals.hh
    stage2_main.hh
    FukushimaLambertW.hh
)
add_executable(stage2.exe
    stage2/AdBuffer.cc
    stage2/CalcsTree.cc
    stage2/Calculator.cc
    stage2/IbdTree.cc
    stage2/Li9Calc.cc
    stage2/MultCut.cc
    stage2/MuonAlg.cc
    stage2/Readers.cc
    stage2/ReCalc.cc
    stage2/Selectors.cc
    stage2/stage2_main.cc
    stage2/stage2_exe.cpp
    stage2/external/FukushimaLambertW.cc
    )

ROOT_GENERATE_DICTIONARY(Dict_stage2 "${STAGE2_HEADERS}" MODULE stage2.exe)
target_link_libraries(stage2.exe PUBLIC common)
target_compile_definitions(stage2.exe PRIVATE IBDSEL_BASE="${IBDSEL_BASE}")
