INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

# The map loading stuff relies on Qt
IF ( QT4_FOUND )

    ADD_EXECUTABLE( testconvert testconvert.cpp )
    TARGET_LINK_LIBRARIES( testconvert OrcaOgMap HydroMapLoad )

    GLOBAL_ADD_TEST( OrcaOgMapConversion testconvert ${CMAKE_CURRENT_SOURCE_DIR}/testmap.png )

ENDIF ( QT4_FOUND )