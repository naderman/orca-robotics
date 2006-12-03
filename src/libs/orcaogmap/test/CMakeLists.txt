INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

INCLUDE_DIRECTORIES( 
  ${PROJECT_SOURCE_DIR}/src/libs
  )

LINK_DIRECTORIES( 
  ${PROJECT_BINARY_DIR}/src/utils/orcaogmap
  )

ADD_EXECUTABLE( testraytrace testraytrace.cpp )
TARGET_LINK_LIBRARIES( testraytrace OrcaIce OrcaOgMap OrcaMisc )
ADD_TEST( OrcaOgMapRayTrace testraytrace "${PROJECT_SOURCE_DIR}/src/libs/orcaogmap/test/reference_output.txt" )
MESSAGE( STATUS "Planning to Build Test     : OrcaOgMapRayTrace [libOrcaOgMap]")

# The map loading stuff relies on Qt
IF ( QT4_FOUND )

  ADD_EXECUTABLE( testconvert testconvert.cpp )
  TARGET_LINK_LIBRARIES( testconvert OrcaOgMap OrcaOgMapLoad )
  ADD_TEST( OrcaOgMapConversion testconvert ${CMAKE_CURRENT_SOURCE_DIR}/testmap.png )
  MESSAGE( STATUS "Planning to Build Test     : OrcaOgMapConversion [libOrcaOgMap]")

ENDIF ( QT4_FOUND )