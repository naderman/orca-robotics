OPTION( BUILD_ORCA_MONITOR "Try to build Orca Monitor" ON )

IF ( NOT WIN32 AND BUILD_ORCA_MONITOR )

INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

#
# This gathers all the source files into the variable 'SRCS'
#
FILE( GLOB SRCS *.cpp )

GLOBAL_ADD_EXECUTABLE( orcamon ${SRCS} )

GENERATE_FROM_DEF( orcamon.def )

ENDIF ( NOT WIN32 AND BUILD_ORCA_MONITOR )
