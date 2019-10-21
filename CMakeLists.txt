cmake_minimum_required(VERSION 3.5)

project(IbdSel)
set(LIBNAME IbdSel)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_COMPILER, "clang++")
set(CMAKE_CXX_STANDARD, 14)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Setting ${CMAKE_BUILD_TYPE} build")

FILE(GLOB SOURCES selector/**.cc)
FILE(GLOB HEADERS selector/**.cc)
message(STATUS "Sources: ${SOURCES}")
message(STATUS "HEADERS: ${HEADERS}")

include_directories(
    selector/SelectorFramework/core
    selector/SelectorFramework/samples
    selector/
)

set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Werror -Wall  -pedantic -pipe")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wall  -pedantic -pipe")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost 1.40.0 REQUIRED)
if(Boost_FOUND)
    message(STATUS "found Boost version ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION} in ${Boost_INCLUDE_DIRS}")
else(Boost_FOUND)
    message(FATAL_ERROR "Boost >= 1.40.0 not found")
endif(Boost_FOUND)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_package(ROOT REQUIRED)
link_directories(${ROOT_LIBRARY_DIR})

#ROOT_GENERATE_DICTIONARY(Dict "${HEADERS}" OPTIONS -interpreteronly)
#add_library(${LIBNAME} SHARED ${SOURCES} Dict.cxx)
add_library(${LIBNAME} SHARED ${SOURCES})

if(result)
    set_property(TARGET ${LIBNAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_include_directories(${LIBNAME} SYSTEM PRIVATE ${ROOT_INCLUDE_DIR})
target_include_directories(${LIBNAME} SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${LIBNAME} PRIVATE  ${ROOT_LIBRARIES} -lPyROOT)

