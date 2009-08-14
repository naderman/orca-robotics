include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )

add_executable( testraytrace testraytrace.cpp )
target_link_libraries( testraytrace HydroOgMap HydroUtil )
GBX_ADD_TEST( HydroOgMapRayTrace testraytrace "${PROJECT_SOURCE_DIR}/src/libs/hydroogmap/test/reference_output.txt" )
