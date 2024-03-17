cmake_minimum_required(VERSION 3.22)

project(wkgreom VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-g)

LIST(APPEND CMAKE_CTEST_ARGUMENTS "--verbose")

include(CTest)
enable_testing()

find_package(fmt)

add_subdirectory(wkgeom)
add_subdirectory(capi)
add_subdirectory(test)