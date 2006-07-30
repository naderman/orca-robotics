OPTION( BUILD_PATHPLANNER "Try to build pathplanner" ON )

IF ( NOT WIN32 )
# Needs Qt4 for displaying debug stuff
IF ( BUILD_PATHPLANNER AND QT4_FOUND )

    INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )
    
    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS
            algohandler
            astardriver
            component
            fakedriver
            skeletongraphicsI
            skeletondriver
            pathplanner2dI
            gridpotentialdriver
    )

    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}
        ${PROJECT_SOURCE_DIR}/src/utils
    )

    ADD_DEFINITIONS( ${QT_DEFINITIONS} )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( pathplanner main ${COMMON_SRCS} )
    TARGET_LINK_LIBRARIES( pathplanner OrcaOgMap OrcaPathPlan OrcaMisc  ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
    
    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaPathPlanner SHARED service ${COMMON_SRCS} )

    GENERATE_CONFIG_FILE( pathplanner.def )

ELSE ( BUILD_PATHPLANNER AND QT4_FOUND )

    MESSAGE("Not Planning to Build Pathplanner -- Qt4 not found")

ENDIF ( BUILD_PATHPLANNER AND QT4_FOUND )
ENDIF ( NOT WIN32 )
