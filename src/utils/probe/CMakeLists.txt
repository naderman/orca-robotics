OPTION( BUILD_PROBE "Try to build Probe" ON )

IF ( BUILD_PROBE )

    INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )

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
#     OPTIONAL_SUB_LIBRARY( 
#         "Qt driver for Probe"
#         qt
#         OrcaRegViewQt
#         REGVIEW_QT_LIBS
#         QT4_OK
#         HAVE_QT_DRIVER
#         QorcaCm ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( probe ${SRCS} )
    TARGET_LINK_LIBRARIES( probe OrcaCompModel IceGrid ${PROBE_IOSTREAM_LIBS} )

    GENERATE_CONFIG_FILE( probe.def )

ENDIF ( BUILD_PROBE )
