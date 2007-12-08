SET ( COMP_NAME tracermon )
SET ( SERVICE_NAME OrcaFaithLocaliser )
SET ( OPTION_NAME BUILD_TRACERMON )

SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

OPTION( ${OPTION_NAME} "Try to build ${COMP_NAME}" ON )
IF ( BUILD AND NOT ${OPTION_NAME}  )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT ${OPTION_NAME} )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    # keyboard driver with iostream
    SET ( ALL_FOUND 1 )
    OPTIONAL_SUB_LIBRARY(
        "Terminal Iostream driver for TracerMon"
        term-iostream
        OrcaTracerMonTermIostream
        TERM_IOSTREAM_LIBS
        ALL_FOUND
        HAVE_TERM_IOSTREAM_DRIVER
        # no dependencies
    )

# curses exist in QNX but apparently assume_default_colors() is not supported
IF ( OS_LINUX )
    INCLUDE( ${CMAKE_ROOT}/Modules/FindCurses.cmake )
    IF ( CURSES_INCLUDE_DIR )
        MESSAGE( STATUS "Looking for libncurses - found")
        SET( CURSES_FOUND 1 CACHE INTERNAL "libncurses" )
    ELSE ( CURSES_INCLUDE_DIR )
        MESSAGE( STATUS "Looking for libncurses - not found")
        SET( CURSES_FOUND 0 CACHE INTERNAL "libncurses" )
    ENDIF ( CURSES_INCLUDE_DIR )

    # keyboard driver with libncurses
    OPTIONAL_SUB_LIBRARY(
        "Terminal Ncurses driver for TracerMon"
        term-ncurses
        OrcaTracerMonTermNcurses
        TERM_NCURSES_LIBS
        CURSES_FOUND
        HAVE_TERM_NCURSES_DRIVER
        ncurses form
    )
ENDIF ( OS_LINUX )

    FILE( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS ${TERM_IOSTREAM_LIBS} ${TERM_NCURSES_LIBS} )

    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
