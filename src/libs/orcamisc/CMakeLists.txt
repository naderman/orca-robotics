INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

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
  #ADD_DEFINITIONS( -D__WIN32__ )
ENDIF(NOT OS_WIN)

TARGET_LINK_LIBRARIES ( OrcaMisc OrcaIce OrcaObjects )
IF( OS_QNX )
    TARGET_LINK_LIBRARIES ( OrcaMisc m )
ENDIF( OS_QNX )

INSTALL_FILES( /include/orcamisc .*\\.h$ )

IF ( BUILD_TESTS )
  ADD_SUBDIRECTORY( test )
ENDIF ( BUILD_TESTS )
