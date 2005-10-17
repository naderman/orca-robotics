#
# Do not include /config/compile_flags.cmake here. It will lead to circular dependencies.
#

INCLUDE( ${PROJ_SOURCE_DIR}/src/interfaces/slice/orca/slice2cpp_rules.cmake )

#
# Since CMake doesn't know how to automatically track dependencies for .ice files, 
# these have to be entered manually such that the depended-on files are listed first.
#
SET( SLICE_SOURCES 
  orca.ice
  bros1.ice
  image.ice
  laserdata.ice
  ogmap.ice
  opaque.ice
  pose2d.ice
  stringobj.ice
  binaryindicator.ice
  laser.ice
  position2d.ice
  platform2d.ice
  power.ice
  )

SET( HUMAN_SOURCES )

ADD_SLICE_LIB_RULES( OrcaInterfaces "${SLICE_SOURCES}" "${HUMAN_SOURCES}" )

INCLUDE_DIRECTORIES( BEFORE
    ${ICE_HOME}/include
#    ${PROJ_SOURCE_DIR}/src/interfaces/slice
    ${PROJ_BINARY_DIR}/src/interfaces/slice
    ${PROJ_BINARY_DIR}/src/interfaces/slice/orca
)

LINK_DIRECTORIES(
    ${ICE_HOME}/lib
)

ADD_DEFINITIONS( "-Wall" )
