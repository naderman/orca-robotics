IF ( NOT OS_WIN )

OPTION( BUILD_PATHPLANNER "Try to build pathplanner" ON )

IF ( NOT QT4_FOUND )
  MESSAGE( STATUS "     Qt4 not found: skeletongraphics will not be built" )
ENDIF( NOT QT4_FOUND )

IF ( BUILD_PATHPLANNER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( PathPlannerSources )

    IF ( QT4_FOUND )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
        ADD_DEFINITIONS( -DQT4_FOUND )
        # stand-alone component  
        GLOBAL_ADD_EXECUTABLE( pathplanner main skeletongraphicsI )
    ELSE( QT4_FOUND )
        # stand-alone component  
        GLOBAL_ADD_EXECUTABLE( pathplanner main  )
    ENDIF( QT4_FOUND )
    
    TARGET_LINK_LIBRARIES( pathplanner PathPlannerSources OrcaOgMap OrcaMisc OrcaPathPlan)

    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaPathPlanner SHARED service )
    TARGET_LINK_LIBRARIES( OrcaPathPlanner PathPlannerSources OrcaOgMap OrcaMisc OrcaPathPlan )
    
    GENERATE_CONFIG_FILE( pathplanner.def )

ENDIF ( BUILD_PATHPLANNER )
ENDIF ( NOT OS_WIN )
