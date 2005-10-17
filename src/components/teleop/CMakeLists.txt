OPTION( BUILD_TELEOP "Try to build Teleop" ON )

IF ( BUILD_TELEOP )

  INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
  INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )

  #
  # This gathers all the .cpp files into the variable 'SRCS'
  # NOTE: globbing for {*.cpp,*.cc} should work but does not.
  FILE( GLOB SRCS *.cpp )
#FILE( GLOB SRCS2 *.cc )

  GLOBAL_ADD_EXECUTABLE( teleop ${SRCS} )

ENDIF ( BUILD_TELEOP )
