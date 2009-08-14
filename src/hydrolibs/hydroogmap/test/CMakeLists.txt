include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

add_executable( testraytrace testraytrace.cpp )
target_link_libraries( testraytrace HydroOgMap HydroUtil )
GBX_ADD_TEST( HydroOgMapRayTrace testraytrace "${CMAKE_CURRENT_SOURCE_DIR}/reference_output.txt" )
