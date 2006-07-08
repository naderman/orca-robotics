OPTION( BUILD_LASER_MONITOR "Try to build Laser Monitor" ON )

IF ( BUILD_LASER_MONITOR )

  INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
  INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )

  #
  # This gathers all the source files into the variable 'SRCS'
  #
  FILE( GLOB SRCS *.cpp )

  GLOBAL_ADD_EXECUTABLE( lasermon ${SRCS} )

  GENERATE_FROM_DEF( lasermon.def )

ENDIF ( BUILD_LASER_MONITOR )
