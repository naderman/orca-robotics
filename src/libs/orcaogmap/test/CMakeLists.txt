include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

set( build_test TRUE )

GBX_REQUIRE_VAR( build_test LIB ${LIB_NAME} QT4_FOUND "Windows OS is not  supported" )

set( dep_libs HydroMapLoad )
GBX_REQUIRE_LIBS( build_test LIB ${LIB_NAME} ${dep_libs} )

# The map loading stuff relies on Qt
if( build_test )

    add_executable( testconvert testconvert.cpp )
    target_link_libraries( testconvert OrcaOgMap HydroMapLoad )

    GBX_ADD_TEST( OrcaOgMap_Conversion testconvert ${CMAKE_CURRENT_SOURCE_DIR}/testmap.png )

endif( build_test )