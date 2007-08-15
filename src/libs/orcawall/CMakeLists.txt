IF ( NOT OS_WIN )

  INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
#   INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
  INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

  FILE( GLOB SRCS *.cpp )
  
  GLOBAL_ADD_LIBRARY( OrcaWall SHARED ${SRCS} )
  
  INSTALL_FILES( /include/orcawall .*\\.h$ ) 

ENDIF ( NOT OS_WIN )
