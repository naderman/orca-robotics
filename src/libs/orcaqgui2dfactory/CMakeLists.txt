IF ( NOT OS_WIN )
IF ( QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    SET( MOC_HEADERS
        pathfollower2delement.h
        pathinput.h
        pathplanner2delement.h
    )

    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( OrcaQGui2dFactory SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( OrcaQGui2dFactory
                            OrcaQCompModel 
                            OrcaLog
                            OrcaNavUtil
                            OrcaQGui2d
                            OrcaQGui )

    INSTALL_FILES( /include/orcaqgui2dfactory .*\\.h$ )
    INSTALL_FILES( /share/orcaqgui/ ogHazardLookupTable.png )
    
ELSE  ( QT4_FOUND )

    CANNOT_BUILD_MESSAGE( libOrcaQGui2dFactory "QT4 not found" )

ENDIF ( QT4_FOUND )
ENDIF ( NOT OS_WIN )
