SET_COMPONENT_NAMES ( TracerMon )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD EXE ${APP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( BUILD EXE ${APP_NAME} NOT_OS_WIN "Windows OS is not  supported" )

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
    SET( DEP_LIBS ${TERM_IOSTREAM_LIBS} ${TERM_NCURSES_LIBS} OrcaObjects )

    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${DEP_LIBS} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
