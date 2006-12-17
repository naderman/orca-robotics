IF ( NOT OS_WIN )

  INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
  INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

  FILE( GLOB SRCS *.cpp )
  
  GLOBAL_ADD_LIBRARY( OrcaProbe SHARED ${SRCS} )
  
  INSTALL_FILES( /include/orcaprobe .*\\.h$ ) 

ENDIF ( NOT OS_WIN )
