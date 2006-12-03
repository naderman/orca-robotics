IF ( NOT WIN32 )

  INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
  INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

  FILE( GLOB SRCS *.cpp )
  
  GLOBAL_ADD_LIBRARY( OrcaLog SHARED ${SRCS} )
  
  INSTALL_FILES( /include/orcalog .*\\.h$ ) 

IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )

ENDIF ( NOT WIN32 )
