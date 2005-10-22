#
# Do not include /config/compile_flags.cmake here. It will lead to circular dependencies.
#

INCLUDE( ${PROJ_SOURCE_DIR}/config/slice2cpp_rules.cmake )

#
# Since CMake doesn't know how to automatically track dependencies for .ice files, 
# these have to be entered manually such that the depended-on files are listed first.
#
SET( SLICE_SOURCES 
  orca.ice
  bros1.ice
  image.ice
  ogmap.ice
  opaque.ice
  stringobj.ice
  binaryindicator.ice
  laser.ice
  position2d.ice
  platform2d.ice
  power.ice
  )

#
# Work out the list of generated files from the list of slice sources
#
GENERATE_SLICE_RULES( SLICE_CPP_FILES SLICE_HEADER_FILES ${SLICE_SOURCES} )

ADD_LIBRARY( OrcaInterfaces SHARED ${SLICE_CPP_FILES} )
INSTALL_TARGETS( /lib OrcaInterfaces )
INSTALL_FILES( /include/orca FILES ${SLICE_HEADER_FILES} )

INCLUDE_DIRECTORIES( BEFORE
    ${ICE_HOME}/include
    ${PROJ_BINARY_DIR}/src/interfaces/slice
    ${PROJ_BINARY_DIR}/src/interfaces/slice/orca
)

ADD_DEFINITIONS( "-Wall" )
