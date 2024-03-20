find_package(Catch2 3 REQUIRED)
add_executable(tests test_wkgeom.cpp)

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

target_link_libraries(tests 
    PRIVATE Catch2::Catch2WithMain
    PRIVATE fmt::fmt
    PRIVATE spdlog::spdlog_header_only
    PRIVATE wkgeom
)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)
include(Catch)
catch_discover_tests(tests
  PROPERTIES
  LABELS
)
