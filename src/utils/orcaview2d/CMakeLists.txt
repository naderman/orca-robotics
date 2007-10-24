OPTION( BUILD_ORCAVIEW2D_GUI "Try to build OrcaView2d GUI" ON )

IF ( NOT OS_WIN )
IF ( BUILD_ORCAVIEW2D_GUI AND QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( orcaview2d ${HUMAN_SRCS} )
    TARGET_LINK_LIBRARIES( orcaview2d OrcaQCompModel OrcaQGui OrcaQGui2d HydroDll )

    GENERATE_CONFIG_FILE( orcaview2d.def )

ELSE ( BUILD_ORCAVIEW2D_GUI AND QT4_FOUND )

    MESSAGE( STATUS "Not Planning to Build Utility: OrcaView -- Qt4 not found")
    
ENDIF ( BUILD_ORCAVIEW2D_GUI AND QT4_FOUND )
ENDIF ( NOT OS_WIN )
