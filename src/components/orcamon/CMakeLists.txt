OPTION( BUILD_ORCA_MONITOR "Try to build Orca Monitor" ON )

IF ( NOT WIN32 AND BUILD_ORCA_MONITOR )

INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

#
# This gathers all the source files into the variable 'SRCS'
#
FILE( GLOB SRCS *.cpp )

GLOBAL_ADD_EXECUTABLE( orcamon ${SRCS} )

GENERATE_CONFIG_FILE( orcamon.def )

ENDIF ( NOT WIN32 AND BUILD_ORCA_MONITOR )
