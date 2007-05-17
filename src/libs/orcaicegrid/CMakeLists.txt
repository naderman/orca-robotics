OPTION( BUILD_ORCAICEGRID "Try to build libOrcaIceGrid" ON )

IF ( BUILD_ORCAICEGRID )

  INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

  FILE( GLOB SRCS *.cpp )

  IF ( NOT OS_WIN )
	GLOBAL_ADD_LIBRARY( OrcaIceGrid SHARED ${SRCS} )
  ELSE ( NOT OS_WIN )
	GLOBAL_ADD_LIBRARY( OrcaIceGrid STATIC ${SRCS} )
  ENDIF ( NOT OS_WIN )

  INSTALL_FILES( /include/orcaicegrid .*\\.h$ )

ENDIF ( BUILD_ORCAICEGRID )