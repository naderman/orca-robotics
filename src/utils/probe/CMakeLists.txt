SET ( COMP_NAME probe )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${COMP_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( EXE ${COMP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )

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

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${SRCS} )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} OrcaCompModel IceGrid OrcaProbe HydroDll 
            ${PROBE_TERM_IOSTREAM_LIBS} ${PROBE_GUI_QT_LIBS} )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
