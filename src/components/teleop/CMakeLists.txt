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
        HAVE_KEYBOARD_DRIVER
    )

    # continue only if we can build at least one driver
    IF ( HAVE_KEYBOARD_DRIVER )
    
        # This gathers all the .cpp files into the variable 'SRCS'
        FILE( GLOB SRCS *.cpp )
        
        GLOBAL_ADD_EXECUTABLE( teleop ${SRCS} )
    
    ENDIF ( HAVE_KEYBOARD_DRIVER )

ENDIF ( BUILD_TELEOP )
