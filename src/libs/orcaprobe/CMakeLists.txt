IF ( NOT WIN32 )

  INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
  INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

  FILE( GLOB SRCS *.cpp )
  
  GLOBAL_ADD_LIBRARY( OrcaProbe SHARED ${SRCS} )
  
  INSTALL_FILES( /include/orcaprobe .*\\.h$ ) 

ENDIF ( NOT WIN32 )
