OPTION( BUILD_ORCAICEGRID "Try to build libOrcaIceGrid" ON )

IF ( BUILD_ORCAICEGRID )

  INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

  FILE( GLOB SRCS *.cpp )

  GLOBAL_ADD_LIBRARY( OrcaIceGrid SHARED ${SRCS} )

  INSTALL_FILES( /include/orcaicegrid .*\\.h$ )

ENDIF ( BUILD_ORCAICEGRID )