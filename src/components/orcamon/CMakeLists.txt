IF( NOT OS_WIN )

OPTION( BUILD_ORCA_MONITOR "Try to build Orca Monitor" ON )

IF( BUILD_ORCA_MONITOR )

INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

#
# This gathers all the source files into the variable 'SRCS'
#
FILE( GLOB SRCS *.cpp )

GLOBAL_ADD_EXECUTABLE( orcamon ${SRCS} )

GENERATE_CONFIG_FILE( orcamon.def )

ENDIF ( BUILD_ORCA_MONITOR )
ENDIF ( NOT OS_WIN )
