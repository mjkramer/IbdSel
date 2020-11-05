cmake_minimum_required(VERSION 3.12)

set(COMMON_HEADERS 
    AdTree.hh
    Constants.hh
    Misc.hh
    MuonTree.hh
    )

add_library(common SHARED AdTree.cc Misc.cc MuonTree.cc)
ROOT_GENERATE_DICTIONARY(Dict_common "${COMMON_HEADERS}" MODULE common OPTIONS -interpreteronly)
target_link_libraries(common PUBLIC SelectorFramework)
target_include_directories(common PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
install_headers(common)
