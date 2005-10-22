OPTION( BUILD_TELEOP "Try to build Teleop" ON )

IF ( BUILD_TELEOP )

  INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
  INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )

    # keyboard driver
    OPTIONAL_SUB_LIBRARY(
        "Keyboard driver for Teleop"
        keyboard
        OrcaTeleopKeyboard
        HAVE_TERMIO
        -DHAVE_KEYBOARD_DRIVER
         )

#IF ( HAVE_TERMIO )
#    MESSAGE("    Keyboard driver for Teleop - ok")
#    SUBDIRS( keyboard )
#    LINK_LIBRARIES( OrcaTeleopKeyboard )
#   LINK_DIRECTORIES( ${PROJ_BINARY_DIR}/src/components/teleop/keyboard )
#   ADD_DEFINITIONS( -DHAVE_KEYBOARD_DRIVER )
#ELSE ( HAVE_TERMIO )
#   MESSAGE("    Keyboard driver for Teleop - cannot")
#ENDIF ( HAVE_TERMIO )

# continue only if we can build at least one driver
IF ( HAVE_TERMIO )


  # This gathers all the .cpp files into the variable 'SRCS'
  FILE( GLOB SRCS *.cpp )

  GLOBAL_ADD_EXECUTABLE( teleop ${SRCS} )

ENDIF ( HAVE_TERMIO )
ENDIF ( BUILD_TELEOP )
