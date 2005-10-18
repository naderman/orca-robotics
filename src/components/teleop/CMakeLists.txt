OPTION( BUILD_TELEOP "Try to build Teleop" ON )

IF ( BUILD_TELEOP )

  SUBDIRS( keyboard )

  INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
  INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )

  # This gathers all the .cpp files into the variable 'SRCS'
  FILE( GLOB SRCS *.cpp )

  LINK_DIRECTORIES( ${PROJ_BINARY_DIR}/src/components/teleop/keyboard )
  LINK_LIBRARIES( TeleopKeyboard )

  GLOBAL_ADD_EXECUTABLE( teleop ${SRCS} )

ENDIF ( BUILD_TELEOP )
