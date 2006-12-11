INCLUDE_DIRECTORIES( 
  ${PROJECT_SOURCE_DIR} # for config.h
  ${PROJECT_SOURCE_DIR}/src/libs
)

FILE( GLOB SRCS *.cpp )

IF(NOT WIN32)
  ADD_DEFINITIONS( -Wall )
  GLOBAL_ADD_LIBRARY( OrcaMisc SHARED ${SRCS} )
ELSE(NOT WIN32)
  GLOBAL_ADD_LIBRARY( OrcaMisc STATIC ${SRCS} )
  # windows: turn off deprecation warnings  
  ADD_DEFINITIONS( "-Wall -D_CRT_SECURE_NO_DEPRECATE" )
ENDIF(NOT WIN32)

INSTALL_FILES( /include/orcamisc .*\\.h$ )
