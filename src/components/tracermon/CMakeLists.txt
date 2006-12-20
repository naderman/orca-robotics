IF ( NOT OS_WIN )

OPTION( BUILD_TRACERMON "Try to build TracerMon" ON )

IF ( BUILD_TRACERMON )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

#     # keyboard driver with termio
#     OPTIONAL_SUB_LIBRARY(
#         "KeyboardTermio driver for Teleop"
#         kbd-termio
#         OrcaTeleopKeyboardTermio
#         KBD_TERMIO_LIBS
#         HAVE_TERMIO_H
#         HAVE_KEYBOARD_TERMIO_DRIVER
#     )
# 
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
#     
#     # joystick driver
#     OPTIONAL_SUB_LIBRARY(
#         "Joystick driver for Teleop"
#         joystick
#         OrcaTeleopJoystick
#         JOY_LIBS
#         HAVE_LINUX_INPUT_H
#         HAVE_JOYSTICK_DRIVER
#     )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_EXECUTABLE( tracermon ${SRCS} )
    TARGET_LINK_LIBRARIES( tracermon ) #${JOY_LIBS} ${KBD_TERMIO_LIBS} ${KBD_NCURSES_LIBS} )

    GENERATE_CONFIG_FILE( tracermon.def )

ENDIF ( BUILD_TRACERMON )
ENDIF ( NOT OS_WIN )
