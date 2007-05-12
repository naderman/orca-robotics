IF ( OS_LINUX )

  INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
  INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
  
  FILE( GLOB SRCS *.cpp )
  
  GLOBAL_ADD_LIBRARY( OrcaWifi SHARED ${SRCS} )
  
  INSTALL_FILES( /include/orcawifi .*\\.h$ )
  
  IF ( BUILD_TESTS ) 
    ADD_SUBDIRECTORY ( test )
  ENDIF ( BUILD_TESTS )

ENDIF ( OS_LINUX )


