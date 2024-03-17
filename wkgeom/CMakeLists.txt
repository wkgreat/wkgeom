add_library(wkgeom SHARED)

set(sources 
    PRIVATE wkgeom.cpp)

target_sources(wkgeom ${sources})

target_link_libraries(wkgeom fmt::fmt)

set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/lib)