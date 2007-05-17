IF ( NOT OS_WIN )

  INCLUDE_DIRECTORIES( 
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/src/libs
    )

  FILE( GLOB SRCS *.cpp )

  GLOBAL_ADD_LIBRARY( OrcaSerial SHARED ${SRCS} )

  INSTALL_FILES( /include/orcaserial .*\\.h$ )
  
ENDIF ( NOT OS_WIN )
