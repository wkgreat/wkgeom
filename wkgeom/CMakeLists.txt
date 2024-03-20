add_library(wkgeom SHARED)

set(sources 
    PRIVATE wkgeom.cpp)

target_sources(wkgeom ${sources})

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

target_link_libraries(wkgeom 
    PRIVATE fmt::fmt
    PRIVATE spdlog::spdlog_header_only
)

set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/lib)