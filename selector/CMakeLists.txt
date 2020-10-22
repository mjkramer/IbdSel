cmake_minimum_required(VERSION 3.15)
project(IbdSel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
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

add_subdirectory(SelectorFramework)
add_subdirectory(common)
add_subdirectory(stage1)
add_subdirectory(stage2)
