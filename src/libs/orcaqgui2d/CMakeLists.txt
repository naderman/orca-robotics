IF ( NOT OS_WIN )
IF ( QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    SET( MOC_HEADERS
        worldview.h
        zoomwidget.h
    )

    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( OrcaQGui2d SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( OrcaQGui2d OrcaQCompModel )
    
    INSTALL_FILES( /include/orcaqgui2d .*\\.h$ )

ELSE  ( QT4_FOUND )

    CANNOT_BUILD_MESSAGE( libOrcaQGui2d "QT4 not found")

ENDIF ( QT4_FOUND )
ENDIF ( NOT OS_WIN )
