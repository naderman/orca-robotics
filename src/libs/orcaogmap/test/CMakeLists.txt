INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

ADD_EXECUTABLE( testraytrace testraytrace.cpp )
TARGET_LINK_LIBRARIES( testraytrace OrcaOgMap OrcaMisc )
GLOBAL_ADD_TEST( OrcaOgMapRayTrace testraytrace "${PROJECT_SOURCE_DIR}/src/libs/orcaogmap/test/reference_output.txt" )

# The map loading stuff relies on Qt
IF ( QT4_FOUND )

    ADD_EXECUTABLE( testconvert testconvert.cpp )
    TARGET_LINK_LIBRARIES( testconvert OrcaOgMap OrcaMapLoad )

    GLOBAL_ADD_TEST( OrcaOgMapConversion testconvert ${CMAKE_CURRENT_SOURCE_DIR}/testmap.png )

ENDIF ( QT4_FOUND )