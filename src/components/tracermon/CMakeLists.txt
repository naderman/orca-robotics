ORCA_SET_COMPONENT_NAME( TracerMon )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )

SET( int_libs OrcaObjects )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    # keyboard driver with iostream
    SET( ALL_FOUND 1 )
    ORCA_OPTIONAL_SUB_LIBRARY(
        "Terminal Iostream driver for TracerMon"
        term-iostream
        OrcaTracerMonTermIostream
        TERM_IOSTREAM_LIBS
        ALL_FOUND
        HAVE_TERM_IOSTREAM_DRIVER
        # no dependencies
    )

# curses exist in QNX but apparently assume_default_colors() is not supported
IF( GBX_OS_LINUX )
    INCLUDE( ${CMAKE_ROOT}/Modules/FindCurses.cmake )
    IF( CURSES_INCLUDE_DIR )
        MESSAGE( STATUS "Looking for libncurses - found")
        SET( CURSES_FOUND 1 CACHE INTERNAL "libncurses" )
    ELSE ( CURSES_INCLUDE_DIR )
        MESSAGE( STATUS "Looking for libncurses - not found")
        SET( CURSES_FOUND 0 CACHE INTERNAL "libncurses" )
    ENDIF( CURSES_INCLUDE_DIR )

    # keyboard driver with libncurses
    ORCA_OPTIONAL_SUB_LIBRARY(
        "Terminal Ncurses driver for TracerMon"
        term-ncurses
        OrcaTracerMonTermNcurses
        TERM_NCURSES_LIBS
        CURSES_FOUND
        HAVE_TERM_NCURSES_DRIVER
        ncurses form
    )
ENDIF( GBX_OS_LINUX )

    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${ext_libs} ${int_libs} ${TERM_IOSTREAM_LIBS} ${TERM_NCURSES_LIBS} )

    ORCA_WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF( build )
