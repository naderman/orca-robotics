INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

LINK_LIBRARIES( OrcaGeom2d OrcaMisc )

ADD_EXECUTABLE( circle circle.cpp )
GLOBAL_ADD_TEST( OrcaGeom2dCircle circle )
