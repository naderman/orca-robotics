INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

INCLUDE_DIRECTORIES( 
  ${PROJECT_SOURCE_DIR}/src/libs 
  ${PROJECT_BINARY_DIR} # for config.h
  )

FILE( GLOB SRCS *.cpp )

# for some reason cannot get shared libs working in windows
IF(NOT WIN32)
  GLOBAL_ADD_LIBRARY( OrcaObjects SHARED ${SRCS} )
ELSE (NOT WIN32)
  GLOBAL_ADD_LIBRARY( OrcaObjects STATIC ${SRCS} )
ENDIF (NOT WIN32)

TARGET_LINK_LIBRARIES( OrcaObjects OrcaInterfaces )

# special dependency on the generated files
ADD_DEPENDENCIES( OrcaObjects OrcaInterfaces )

INSTALL_FILES( /include/orcaobj .*\\.h$ )

IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )
