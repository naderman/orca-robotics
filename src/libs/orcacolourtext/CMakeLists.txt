IF ( OS_LINUX )

  FILE( GLOB SRCS *.cpp )

  GLOBAL_ADD_LIBRARY( OrcaColourText SHARED ${SRCS} )

  INSTALL_FILES( /include/orcacolourtext .*\\.h$ )

  IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY( test )
  ENDIF ( BUILD_TESTS )
  
ENDIF ( OS_LINUX )
