OPTION( BUILD_TELEOP "Try to build Teleop" ON )

IF ( BUILD_TELEOP )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )

    # keyboard driver
    OPTIONAL_SUB_LIBRARY(
        "Keyboard driver for Teleop"
        keyboard
        OrcaTeleopKeyboard
        KBD_LIBS
        HAVE_TERMIO_H
        HAVE_KEYBOARD_DRIVER
    )

    # joystick driver
    OPTIONAL_SUB_LIBRARY(
        "Joystick driver for Teleop"
        joystick
        OrcaTeleopJoystick
        JOY_LIBS
        HAVE_LINUX_INPUT_H
        HAVE_JOYSTICK_DRIVER
    )


    IF ( NOT HAVE_KEYBOARD_DRIVER AND NOT HAVE_JOYSTICK_DRIVER )
        MESSAGE("    WARNING: only default 'fake' driver can be built")
    ENDIF ( NOT HAVE_KEYBOARD_DRIVER AND NOT HAVE_JOYSTICK_DRIVER )
    
    # This gathers all the .cpp files into the variable 'SRCS'
    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_EXECUTABLE( teleop ${SRCS} )
    TARGET_LINK_LIBRARIES( teleop ${JOY_LIBS} ${KBD_LIBS} )
    GENERATE_FROM_DEF( teleop.def )

ENDIF ( BUILD_TELEOP )
