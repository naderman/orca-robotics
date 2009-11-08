include_directories( ${PROJECT_SOURCE_DIR}/src/libs )

GBX_ADD_EXECUTABLE( hydroping hydroping.cpp )
target_link_libraries( hydroping HydroPing )
