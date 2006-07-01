OPTION( BUILD_TELEOP "Try to build Teleop" ON )

IF ( NOT WIN32 AND BUILD_TELEOP )

    INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )

    # keyboard driver with termio
    OPTIONAL_SUB_LIBRARY(
        "KeyboardTermio driver for Teleop"
        kbd-termio
        OrcaTeleopKeyboardTermio
        KBD_TERMIO_LIBS
        HAVE_TERMIO_H
        HAVE_KEYBOARD_TERMIO_DRIVER
    )

    # keyboard driver with libncurses
    OPTIONAL_SUB_LIBRARY(
        "KeyboardNcurses driver for Teleop"
        kbd-ncurses
        OrcaTeleopKeyboardNcurses
        KBD_NCURSES_LIBS
        CURSES_FOUND
        HAVE_KEYBOARD_NCURSES_DRIVER
        ncurses
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

    
    IF ( NOT HAVE_KEYBOARD_TERMIO_DRIVER AND NOT HAVE_JOYSTICK_DRIVER )
        IF ( NOT HAVE_KEYBOARD_NCURSES_DRIVER )
            MESSAGE("    WARNING: only default 'fake' driver can be built")
        ENDIF ( NOT HAVE_KEYBOARD_NCURSES_DRIVER )
    ENDIF ( NOT HAVE_KEYBOARD_TERMIO_DRIVER AND NOT HAVE_JOYSTICK_DRIVER )
    
    # This gathers all the .cpp files into the variable 'SRCS'
    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_EXECUTABLE( teleop ${SRCS} )
    TARGET_LINK_LIBRARIES( teleop ${JOY_LIBS} ${KBD_TERMIO_LIBS} ${KBD_NCURSES_LIBS} )
    GENERATE_FROM_DEF( teleop.def )

ENDIF ( NOT WIN32 AND BUILD_TELEOP )
