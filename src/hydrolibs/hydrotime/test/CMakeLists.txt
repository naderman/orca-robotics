include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

link_libraries( HydroTime )

add_executable( hydrotimetest hydrotimetest.cpp )
GBX_ADD_TEST( HydroTimeTest hydrotimetest )
