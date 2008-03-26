SET_COMPONENT_NAMES(Probe )
SET( APP_CTRLC_HANDLER 0 )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET( NOT_GBX_OS_WIN NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} NOT_GBX_OS_WIN "Windows OS is not  supported" )

SET( dep_libs OrcaCompModel OrcaProbe )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

SET( dep_libs ${dep_libs} HydroDll )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    SET( IOSTREAM_FOUND 1 )
    OPTIONAL_SUB_LIBRARY( 
        "terminal display driver for Probe using iostreams"
        term-iostream
        OrcaProbeTermIostream
        PROBE_TERM_IOSTREAM_LIBS
        IOSTREAM_FOUND
        HAVE_TERM_IOSTREAM_DRIVER
        )

    # Qt driver
    IF( QT4_FOUND ) 
        SET( QT4_OK 1 )
        INCLUDE_DIRECTORIES( ${QT_INCLUDES} )
    ENDIF( QT4_FOUND ) 

    OPTIONAL_SUB_LIBRARY( 
        "GUI display driver for Probe using Qt"
        gui-qt
        OrcaProbeGuiQt
        PROBE_GUI_QT_LIBS
        QT4_OK
        HAVE_GUI_QT_DRIVER
        OrcaQt OrcaQCompModel ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )

    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${dep_libs} IceGrid ${PROBE_TERM_IOSTREAM_LIBS} ${PROBE_GUI_QT_LIBS} )

    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF( build )
