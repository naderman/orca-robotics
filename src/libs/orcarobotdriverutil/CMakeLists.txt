INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

INCLUDE_DIRECTORIES( 
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src/libs
  )

FILE( GLOB SRCS *.cpp )
GLOBAL_ADD_LIBRARY( OrcaRobotDriverUtil SHARED ${SRCS} )
INSTALL_FILES( /include/orcarobotdriverutil .*\\.h$ )
