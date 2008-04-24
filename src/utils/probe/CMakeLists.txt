ORCA_SET_COMPONENT_NAME(Probe )
SET( APP_CTRLC_HANDLER 0 )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET( now_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} now_os_win "Windows OS is not  supported" )

SET( ext_libs HydroDll )
GBX_REQUIRE_INSTALLS( build EXE ${APP_NAME} ${ext_libs} )

SET( int_libs OrcaCompModel OrcaProbe )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    SET( IOSTREAM_FOUND 1 )
    ORCA_OPTIONAL_SUB_LIBRARY( 
        "terminal display driver for Probe using iostreams"
        term-iostream
        OrcaProbeTermIostream
        PROBE_TERM_IOSTREAM_LIBS
        IOSTREAM_FOUND
        HAVE_TERM_IOSTREAM_DRIVER
    )
    IF( HAVE_TERM_IOSTREAM_DRIVER )
        SET( opt_libs ${opt_libs} ${PROBE_TERM_IOSTREAM_LIBS} )
    ENDIF( HAVE_TERM_IOSTREAM_DRIVER )

    # Qt driver
    SET( driver_lib_name OrcaProbeGuiQt )
    SET( build_qt_driver TRUE )
    GBX_REQUIRE_VAR( build_qt_driver LIB ${driver_lib_name} QT4_FOUND "Qt4 not found" )

    SET( int_qlibs OrcaQt OrcaQCompModel )
    GBX_REQUIRE_TARGETS( build_qt_driver LIB ${driver_lib_name} ${int_qlibs} )

    IF ( build_qt_driver )

        INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )

        # this old macro will be replaced at some point
        ORCA_OPTIONAL_SUB_LIBRARY( 
            "GUI display driver for Probe using Qt"
            gui-qt
            OrcaProbeGuiQt
            PROBE_GUI_QT_LIBS
            QT4_OK
            HAVE_GUI_QT_DRIVER
            OrcaQt OrcaQCompModel
        )

        SET( opt_libs ${opt_libs} ${PROBE_GUI_QT_LIBS} )
    ENDIF ( build_qt_driver )

    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${int_libs} ${ext_libs} IceGrid ${opt_libs} )

    # build stand-alone component  
    ORCA_WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF( build )
