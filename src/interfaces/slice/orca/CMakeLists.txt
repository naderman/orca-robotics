#
# Do not include /config/compile_flags.cmake here. It will lead to circular dependencies.
#

INCLUDE( ${PROJ_SOURCE_DIR}/src/interfaces/slice/orca/slice2cpp_rules.cmake )

#FILE( GLOB SLICE_SOURCES *.ice )
SET( SLICE_SOURCES 
  image.ice laser.ice laserdata.ice ogmap.ice opaque.ice orca.ice platform2d.ice pose2d.ice position2d.ice position2ddata.ice power.ice powerdata.ice stringobj.ice velocity2ddata.ice
  )

SET( HUMAN_SOURCES )

ADD_SLICE_LIB_RULES( OrcaInterfaces "${SLICE_SOURCES}" "${HUMAN_SOURCES}" )

##
## Dodgy way of generating from .ice files.
## TODO: fix me.
##
## This only runs once on 'cmake' not on 'make'
##
#FILE( GLOB ICE_SRCS *.ice )
#EXEC_PROGRAM( slice2cpp ${CMAKE_CURRENT_SOURCE_DIR} ARGS "-I.. *.ice" )


INCLUDE_DIRECTORIES( BEFORE
    ${ICE_HOME}/include
    ${PROJ_SOURCE_DIR}/src/interfaces/slice
    ${PROJ_SOURCE_DIR}/src/interfaces/slice/orca
)

LINK_DIRECTORIES(
    ${ICE_HOME}/lib
)

ADD_DEFINITIONS( "-Wall" )

#FILE( GLOB SRCS *.cpp )
#
#ADD_LIBRARY( OrcaInterfaces SHARED ${SRCS} )
#
#INSTALL_TARGETS( /lib OrcaInterfaces )
#
#INSTALL_FILES( /include/orca .*\\.h$ )
