INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

#
# alexm: we don't need all of Ice to get random numbers!
#
LINK_LIBRARIES( OrcaGeom2d  )

ADD_EXECUTABLE( circle circle.cpp )
GLOBAL_ADD_TEST( OrcaGeom2dCircle circle )
