ORCA_SET_COMPONENT_NAME( TracerMon )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )

set( dep_libs OrcaIfaceUtil OrcaObjects )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    # Obsolete but still used here
    include( ${ORCA_CMAKE_DIR}/OptionalSubLibrary.cmake )

    # keyboard driver with iostream
    set( ALL_FOUND 1 )
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
if( GBX_OS_LINUX )
    include( ${CMAKE_ROOT}/Modules/FindCurses.cmake )
    if( CURSES_INCLUDE_DIR )
        message( STATUS "Looking for libncurses - found")
        set( CURSES_FOUND 1 CACHE INTERNAL "libncurses" )
    else( CURSES_INCLUDE_DIR )
        message( STATUS "Looking for libncurses - not found")
        set( CURSES_FOUND 0 CACHE INTERNAL "libncurses" )
    endif( CURSES_INCLUDE_DIR )

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
endif( GBX_OS_LINUX )

    file( GLOB srcs *.cpp )
    set( dep_libs ${dep_libs} ${TERM_IOSTREAM_LIBS} ${TERM_NCURSES_LIBS} )

    ORCA_WRAP_COMPONENT_INTO_APP( ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
