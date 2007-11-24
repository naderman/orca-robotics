INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseHydroUtil.cmake )

LINK_LIBRARIES( OrcaGeom2d  )

ADD_EXECUTABLE( circle circle.cpp )
GLOBAL_ADD_TEST( OrcaGeom2dCircle circle )
