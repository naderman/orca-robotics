OPTION( BUILD_ORCAVIEW3D_GUI "Try to build OrcaView3d GUI" OFF )

IF ( BUILD_ORCAVIEW3D_GUI )

    IF ( NOT OS_WIN AND QT4_FOUND AND GLUT_FOUND )
    
        INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
        #
        # This gathers all the source files into the variable 'SRCS'
        #
        FILE( GLOB HUMAN_SRCS *.cpp )
        
        GLOBAL_ADD_EXECUTABLE( orcaview3d ${HUMAN_SRCS} )
        TARGET_LINK_LIBRARIES( orcaview3d OrcaQCompModel OrcaQGui OrcaQGui3d OrcaDynamicLoad )
    
        GENERATE_CONFIG_FILE( orcaview3d.def )
    
        IF ( LIB3DS_FOUND )
        ADD_DEFINITIONS( -DHAVE_LIB3DS )
        ENDIF ( LIB3DS_FOUND )
    
    ELSE ( NOT OS_WIN AND QT4_FOUND AND GLUT_FOUND )
    
        MESSAGE( STATUS "Not Planning to Build Utility: OrcaView3d -- Qt4 or GLUT not found")
        
    ENDIF ( NOT OS_WIN AND QT4_FOUND AND GLUT_FOUND )

ELSE ( BUILD_ORCAVIEW3D_GUI )
    
    MESSAGE( STATUS "Not Planning to Build Utility: OrcaView3d -- turned off")

ENDIF ( BUILD_ORCAVIEW3D_GUI )
