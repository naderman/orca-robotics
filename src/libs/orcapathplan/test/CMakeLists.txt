INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

ADD_EXECUTABLE( testconversions testconversions.cpp )
TARGET_LINK_LIBRARIES( testconversions OrcaIce OrcaPathPlan OrcaMisc)
ADD_TEST( OrcaPathPlanConversion testconversions )
MESSAGE( STATUS "Planning to Build Test      : OrcaPathPlanConversion [libOrcaPathPlan]")

# The map loading stuff relies on Qt
IF ( QT4_FOUND )

  ADD_EXECUTABLE( testpathplan testpathplan.cpp )
  TARGET_LINK_LIBRARIES( testpathplan OrcaPathPlan OrcaMapLoad )
  ADD_TEST( OrcaPathPlan testpathplan ${CMAKE_CURRENT_SOURCE_DIR}/testmap.png )
  MESSAGE( STATUS "Planning to Build Test      : OrcaPathPlan [libOrcaPathPlan]")

ENDIF ( QT4_FOUND )