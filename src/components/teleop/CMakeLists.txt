IF ( NOT OS_WIN )

OPTION( BUILD_TELEOP "Try to build Teleop" ON )

IF ( BUILD_TELEOP )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    # keyboard driver with termio
    OPTIONAL_SUB_LIBRARY(
        "KeyboardTermio driver for Teleop"
        kbd-termio
        OrcaTeleopKeyboardTermio
        KBD_TERMIO_LIBS
        HAVE_TERMIO_H
        HAVE_KEYBOARD_TERMIO_DRIVER
    )

#     # keyboard driver with libncurses
#     OPTIONAL_SUB_LIBRARY(
#         "KeyboardNcurses driver for Teleop"
#         kbd-ncurses
#         OrcaTeleopKeyboardNcurses
#         KBD_NCURSES_LIBS
#         CURSES_FOUND
#         HAVE_KEYBOARD_NCURSES_DRIVER
#         ncurses
#     )
    
    # joystick driver
    OPTIONAL_SUB_LIBRARY(
        "Joystick driver for Teleop"
        joystick
        OrcaTeleopJoystick
        JOY_LIBS
        HAVE_LINUX_INPUT_H
        HAVE_JOYSTICK_DRIVER
    )
    
    # This gathers all the .cpp files into the variable 'SRCS'
    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_EXECUTABLE( teleop ${SRCS} )
    TARGET_LINK_LIBRARIES( teleop ${KBD_TERMIO_LIBS} ${JOY_LIBS} ) #${KBD_NCURSES_LIBS} )
    GENERATE_CONFIG_FILE( teleop.def )

ENDIF ( BUILD_TELEOP )
ENDIF ( NOT OS_WIN )
