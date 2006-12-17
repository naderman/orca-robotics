IF ( NOT OS_WIN )

  FILE( GLOB SRCS *.cpp )
  
  INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )
  
  GLOBAL_ADD_LIBRARY( OrcaDynamicLoad SHARED ${SRCS} )
  
  INSTALL_FILES( /include/orcadynamicload .*\\.h$ ) 

ENDIF ( NOT OS_WIN )
