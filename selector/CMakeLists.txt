cmake_minimum_required(VERSION 3.15)
project(IbdSel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_SKIP_BUILD_RPATH ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

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

include(CMakePrintHelpers)

function(install_headers target_name)
    file(GLOB_RECURSE INSTALL_HEADERS ${CMAKE_CURRENT_SOURCE_DIR} "*.hh")
    set_property(TARGET ${target_name} APPEND PROPERTY PUBLIC_HEADER "${INSTALL_HEADERS}")
endfunction()

add_subdirectory(SelectorFramework)
add_subdirectory(common)
add_subdirectory(stage1)
add_subdirectory(stage2)

install(TARGETS SelectorFramework common stage1 stage2
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include)
install(TARGETS stage1.exe stage2.exe
    RUNTIME DESTINATION bin)

install(FILES ${CMAKE_BINARY_DIR}/SelectorFramework/libSelectorFramework_rdict.pcm DESTINATION lib)
install(FILES ${CMAKE_BINARY_DIR}/stage1/libstage1_rdict.pcm DESTINATION lib)
install(FILES ${CMAKE_BINARY_DIR}/stage2/libstage2_rdict.pcm DESTINATION lib)
install(FILES ${CMAKE_BINARY_DIR}/common/libcommon_rdict.pcm DESTINATION lib)
