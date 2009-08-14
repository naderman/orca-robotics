ORCA_SET_COMPONENT_NAME(Probe )
set( APP_CTRLC_HANDLER 0 )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( now_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} now_os_win "Windows OS is not  supported" )

set( dep_libs HydroDll OrcaCompModel OrcaProbe )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    # Obsolete but still used here
    include( ${ORCA_CMAKE_DIR}/OptionalSubLibrary.cmake )

    set( IOSTREAM_FOUND 1 )
    ORCA_OPTIONAL_SUB_LIBRARY( 
        "terminal display driver for Probe using iostreams"
        term-iostream
        OrcaProbeTermIostream
        PROBE_TERM_IOSTREAM_LIBS
        IOSTREAM_FOUND
        HAVE_TERM_IOSTREAM_DRIVER
    )
    if( HAVE_TERM_IOSTREAM_DRIVER )
        set( opt_libs ${opt_libs} ${PROBE_TERM_IOSTREAM_LIBS} )
    endif( HAVE_TERM_IOSTREAM_DRIVER )

    # Qt driver
    set( driver_lib_name OrcaProbeGuiQt )
    set( build_qt_driver TRUE )
    GBX_REQUIRE_VAR( build_qt_driver LIB ${driver_lib_name} QT4_FOUND "Qt4 not found" )

    set( dep_qlibs OrcaQt OrcaQCompModel )
    GBX_REQUIRE_LIBS( build_qt_driver LIB ${driver_lib_name} ${dep_qlibs} )

    if( build_qt_driver )

        include( ${ORCA_CMAKE_DIR}/UseQt.cmake )

        # this old macro will be replaced at some point
        ORCA_OPTIONAL_SUB_LIBRARY( 
            "GUI display driver for Probe using Qt"
            gui-qt
            OrcaProbeGuiQt
            PROBE_GUI_QT_LIBS
            QT4_OK
            HAVE_GUI_QT_DRIVER
            ${dep_qlibs}
        )

        set( opt_libs ${opt_libs} ${PROBE_GUI_QT_LIBS} )
    endif( build_qt_driver )

    file( GLOB srcs *.cpp )
    set( dep_libs ${dep_libs} IceGrid ${opt_libs} )

    # build stand-alone component  
    ORCA_WRAP_COMPONENT_INTO_APP( ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
