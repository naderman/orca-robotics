IF ( NOT OS_WIN )

OPTION( BUILD_ORCAVIEW3D_GUI "Try to build OrcaView3d GUI" OFF )

IF ( BUILD_ORCAVIEW3D_GUI )

    IF ( QT4_FOUND AND GLUT_FOUND )
    
        INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
        #
        # This gathers all the source files into the variable 'SRCS'
        #
        FILE( GLOB HUMAN_SRCS *.cpp )
        
        GLOBAL_ADD_EXECUTABLE( orcaview3d ${HUMAN_SRCS} )
        TARGET_LINK_LIBRARIES( orcaview3d OrcaQCompModel OrcaQGui OrcaQGui3d HydroDll )
    
        GENERATE_CONFIG_FILE( orcaview3d.def )
    
        IF ( LIB3DS_FOUND )
        ADD_DEFINITIONS( -DHAVE_LIB3DS )
        ENDIF ( LIB3DS_FOUND )
    
    ELSE ( QT4_FOUND AND GLUT_FOUND )
    
        CANNOT_BUILD_MESSAGE( OrcaView3d "Qt4 or GLUT not found")
        
    ENDIF ( QT4_FOUND AND GLUT_FOUND )

ELSE ( BUILD_ORCAVIEW3D_GUI )
    
    DISABLED_MESSAGE( OrcaView3d )

ENDIF ( BUILD_ORCAVIEW3D_GUI )
ENDIF ( NOT OS_WIN )
