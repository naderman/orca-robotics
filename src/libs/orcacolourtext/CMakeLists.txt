IF ( OS_LINUX )

  FILE( GLOB SRCS *.cpp )

  GLOBAL_ADD_LIBRARY( OrcaColourText SHARED ${SRCS} )

  INSTALL_FILES( /include/orcacolourtext .*\\.h$ )
  
ENDIF ( OS_LINUX )
