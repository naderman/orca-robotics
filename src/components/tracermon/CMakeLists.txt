IF ( NOT OS_WIN )

OPTION( BUILD_TRACERMON "Try to build TracerMon" ON )

IF ( BUILD_TRACERMON )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

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

    GLOBAL_ADD_EXECUTABLE( tracermon ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( tracermon ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( tracermon.def )

ENDIF ( BUILD_TRACERMON )
ENDIF ( NOT OS_WIN )
