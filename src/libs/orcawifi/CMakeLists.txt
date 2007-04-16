IF ( OS_LINUX )

OPTION( BUILD_ORCAWIFI "Try to build libOrcaWifi" ON )

IF ( BUILD_ORCAWIFI )

  INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
  INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
  
  FILE( GLOB SRCS *.cpp )
  
  GLOBAL_ADD_LIBRARY( OrcaWifi SHARED ${SRCS} )
  
  INSTALL_FILES( /include/orcawifi .*\\.h$ )
  
IF ( BUILD_TESTS ) 
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )

ENDIF ( BUILD_ORCAWIFI )

ENDIF ( OS_LINUX )


