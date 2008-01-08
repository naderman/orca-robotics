SET_COMPONENT_NAMES (Probe )
SET ( APP_CTRLC_HANDLER 0 )

REQUIRE_OPTION( EXE ${APP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${APP_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    SET ( IOSTREAM_FOUND 1 )
    OPTIONAL_SUB_LIBRARY( 
        "terminal display driver for Probe using iostreams"
        term-iostream
        OrcaProbeTermIostream
        PROBE_TERM_IOSTREAM_LIBS
        IOSTREAM_FOUND
        HAVE_TERM_IOSTREAM_DRIVER
        )

    # Qt driver
    IF ( QT4_FOUND ) 
        SET ( QT4_OK 1 )
        INCLUDE_DIRECTORIES( ${QT_INCLUDES} )
    ENDIF ( QT4_FOUND ) 

    OPTIONAL_SUB_LIBRARY( 
        "GUI display driver for Probe using Qt"
        gui-qt
        OrcaProbeGuiQt
        PROBE_GUI_QT_LIBS
        QT4_OK
        HAVE_GUI_QT_DRIVER
        OrcaQt OrcaQCompModel ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )

    FILE( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS OrcaCompModel IceGrid OrcaProbe HydroDll 
            ${PROBE_TERM_IOSTREAM_LIBS} ${PROBE_GUI_QT_LIBS} )

    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
