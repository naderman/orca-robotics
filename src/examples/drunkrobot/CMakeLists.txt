OPTION( BUILD_DRUNK_ROBOT "Try to build Drunk Robot Example" ON )

IF ( BUILD_DRUNK_ROBOT )

INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )

#
# This gathers all the source files into the variable 'SRCS'
#
FILE( GLOB SRCS *.cpp )

GLOBAL_ADD_EXECUTABLE( drunkrobot ${SRCS} )

ENDIF ( BUILD_DRUNK_ROBOT )
