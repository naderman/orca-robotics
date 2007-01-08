INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/libs
  )

ADD_EXECUTABLE( circle circle.cpp )
TARGET_LINK_LIBRARIES( circle OrcaGeom2d OrcaMisc )
ADD_TEST( OrcaGeom2dCircle circle )
MESSAGE( STATUS "Planning to Build Test      : OrcaGeom2dCircle [libOrcaGeom2d]")
