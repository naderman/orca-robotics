IF ( NOT OS_WIN )

OPTION( BUILD_TRACERMON "Try to build TracerMon" ON )

IF ( BUILD_TRACERMON )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/tracermon )

    ADD_SUBDIRECTORY( termutil )

    # keyboard driver with iostream
    SET ( ALL_FOUND 1 )
    OPTIONAL_SUB_LIBRARY(
        "Terminal Iostream driver for TracerMon"
        term-iostream
        OrcaTracerMonTermIostream
        TERM_IOSTREAM_LIBS
        ALL_FOUND
        HAVE_TERM_IOSTREAM_DRIVER
        termutil
    )

    # keyboard driver with libncurses
    OPTIONAL_SUB_LIBRARY(
        "Terminal Ncurses driver for TracerMon"
        term-ncurses
        OrcaTracerMonTermNcurses
        TERM_NCURSES_LIBS
        CURSES_FOUND
        HAVE_TERM_NCURSES_DRIVER
        termutil ncurses form
    )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_EXECUTABLE( tracermon ${SRCS} )
    TARGET_LINK_LIBRARIES( tracermon ${TERM_IOSTREAM_LIBS} ${TERM_NCURSES_LIBS} )

    GENERATE_CONFIG_FILE( tracermon.def )

ENDIF ( BUILD_TRACERMON )
ENDIF ( NOT OS_WIN )
