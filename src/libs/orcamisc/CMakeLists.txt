INCLUDE_DIRECTORIES( 
  ${PROJECT_SOURCE_DIR} # for config.h
  ${PROJECT_SOURCE_DIR}/src/libs
)

FILE( GLOB SRCS *.cpp )

IF(NOT OS_WIN)
  ADD_DEFINITIONS( -Wall )
  GLOBAL_ADD_LIBRARY( OrcaMisc SHARED ${SRCS} )
ELSE(NOT OS_WIN)
  GLOBAL_ADD_LIBRARY( OrcaMisc STATIC ${SRCS} )
  # windows: turn off deprecation warnings  
  ADD_DEFINITIONS( "-Wall -D_CRT_SECURE_NO_DEPRECATE" )
  # windows: the stopwatch stuff doesn't compile.
  LIST( REMOVE_ITEM SRCS 
    "${CMAKE_CURRENT_SOURCE_DIR}/stopwatch.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/realtimestopwatch.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/cpustopwatch.cpp" )
ENDIF(NOT OS_WIN)

INSTALL_FILES( /include/orcamisc .*\\.h$ )

IF ( BUILD_TESTS )
  ADD_SUBDIRECTORY( test )
ENDIF ( BUILD_TESTS )
