OPTION( BUILD_PROBE "Try to build Probe" OFF )

IF ( NOT OS_WIN )
IF ( BUILD_PROBE )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    SET ( IOSTREAM_FOUND 1 )
    
    OPTIONAL_SUB_LIBRARY( 
        "IoStream driver for Probe"
        iostream
        OrcaProbeIostream
        PROBE_IOSTREAM_LIBS
        IOSTREAM_FOUND
        HAVE_IOSTREAM_DRIVER
        )

    # Qt driver
    IF ( QT4_FOUND ) 
        SET ( QT4_OK 1 )
        INCLUDE_DIRECTORIES( ${QT_INCLUDES} )
    ENDIF ( QT4_FOUND ) 
    OPTIONAL_SUB_LIBRARY( 
        "Qt driver for Probe"
        qt
        OrcaProbeQt
        PROBE_QT_LIBS
        QT4_OK
        HAVE_QT_DRIVER
        OrcaQt OrcaQCompModel ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( probe ${SRCS} )
    TARGET_LINK_LIBRARIES( probe OrcaCompModel IceGrid OrcaProbe OrcaDynamicLoad ${PROBE_IOSTREAM_LIBS} ${PROBE_QT_LIBS} )

    GENERATE_CONFIG_FILE( probe.def )

ENDIF ( BUILD_PROBE )
ENDIF ( NOT OS_WIN )
