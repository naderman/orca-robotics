IF ( OS_LINUX )

  FILE( GLOB SRCS *.cpp )

  GLOBAL_ADD_LIBRARY( OrcaLockFile SHARED ${SRCS} )

  INSTALL_FILES( /include/orcalockfile .*\\.h$ )

  ADD_SUBDIRECTORY( test )
  
ENDIF ( OS_LINUX )
