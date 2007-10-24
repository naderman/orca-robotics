IF ( NOT OS_WIN )

OPTION( BUILD_ORCAQGUI3DFACTORY_LIBRARY "Try to build OrcaQGui3dFactory library" OFF )

IF ( BUILD_ORCAQGUI3DFACTORY_LIBRARY )

    IF ( QT4_FOUND AND GLUT_FOUND )
    
        INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
        
        FILE( GLOB HUMAN_SRCS *.cpp )
        
        SET( MOC_HEADERS
        )
    
        QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
        
        GLOBAL_ADD_LIBRARY( OrcaQGui3dFactory SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
        
        LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
        TARGET_LINK_LIBRARIES ( OrcaQGui3dFactory
            OrcaQGui
            OrcaQGui3d
            OrcaQCompModel 
            ${QT_QTOPENGL_LIBRARY}
        )
    
        IF ( LIB3DS_FOUND )
        ADD_DEFINITIONS( -DHAVE_LIB3DS )
        TARGET_LINK_LIBRARIES ( OrcaQGui3dFactory 3ds )
        ENDIF ( LIB3DS_FOUND )
    
    ELSE  ( QT4_FOUND AND GLUT_FOUND )
    
        CANNOT_BUILD_MESSAGE( libOrcaQGui3dFactory "QT4 or GLUT not found" )
    
    ENDIF ( QT4_FOUND AND GLUT_FOUND )
    

ELSE ( BUILD_ORCAQGUI3DFACTORY_LIBRARY )

    DISABLED_MESSAGE( libOrcaQGui3dFactory )
    
ENDIF ( BUILD_ORCAQGUI3DFACTORY_LIBRARY )

ENDIF ( NOT OS_WIN )
