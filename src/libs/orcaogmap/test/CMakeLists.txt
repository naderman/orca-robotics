INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

SET( build_test TRUE )

GBX_REQUIRE_VAR( build_test LIB ${LIB_NAME} QT4_FOUND "Windows OS is not  supported" )

SET( dep_libs HydroMapLoad )
GBX_REQUIRE_INSTALLS( build_test LIB ${LIB_NAME} ${dep_libs} )

# The map loading stuff relies on Qt
IF( build_test )

    ADD_EXECUTABLE( testconvert testconvert.cpp )
    TARGET_LINK_LIBRARIES( testconvert OrcaOgMap HydroMapLoad )

    GBX_ADD_TEST( OrcaOgMap_Conversion testconvert ${CMAKE_CURRENT_SOURCE_DIR}/testmap.png )

ENDIF( build_test )