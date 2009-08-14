include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

# this test has external dependencies
set( build TRUE )
set( exe_name extract )

set( dep_libs HydroMapLoad HydroSim2d HydroLaserFeatureExtract )
GBX_REQUIRE_LIBS( build EXE ${exe_name} ${dep_libs} )

if( build )

    add_executable( ${exe_name} extract.cpp )
    target_link_libraries( ${exe_name} ${dep_libs} )
    GBX_ADD_TEST( HydroLfExtract ${exe_name} ${CMAKE_CURRENT_SOURCE_DIR}/testmap.png )

endif( build )