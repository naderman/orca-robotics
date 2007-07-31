IF ( OS_LINUX )

  INCLUDE_DIRECTORIES( 
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/src/libs
    )

  FILE( GLOB SRCS *.cpp )

  GLOBAL_ADD_LIBRARY( OrcaSerial SHARED ${SRCS} )

  INSTALL_FILES( /include/orcaserial .*\\.h$ )

  ADD_SUBDIRECTORY( test )
  
ENDIF ( OS_LINUX )
