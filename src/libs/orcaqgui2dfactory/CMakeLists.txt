IF ( NOT WIN32 AND QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )
    
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}
        ${PROJECT_SOURCE_DIR}/src/libs
    )
    
    ADD_DEFINITIONS( ${QT_DEFINITIONS} )

    #
    # A list of files to MOC
    #
    SET( MOC_HEADERS
        featuremap2delement.h
        ogmapelement.h
        pathfollower2delement.h
        pathplanner2delement.h
    )
        
    #
    # Compile Qt headers
    #
    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( OrcaQGui2dFactory SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( OrcaQGui2dFactory
      OrcaQCompModel 
      ${QT_QTGUI_LIBRARY} 
      ${QT_QTCORE_LIBRARY}
      OrcaQGui2d
      OrcaQGui
    )
    
ELSE  ( NOT WIN32 AND QT4_FOUND )

    MESSAGE( STATUS "Not Planning to Build Utility: libOrcaQGui2dFactory -- QT4 not found")

ENDIF ( NOT WIN32 AND QT4_FOUND )
