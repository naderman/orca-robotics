OPTION( BUILD_GOAL_PLANNER "Try to build GoalPlanner" ON )

IF ( BUILD_GOAL_PLANNER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    # Remove this line if you don't plan to use libOrcaIce
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    SET( COMMON_SRCS
            component.cpp
            localise2dconsumerI.cpp
            mainloop.cpp
            pathfollower2dI.cpp
            pathplanner2dconsumerI.cpp
        )
    
    GLOBAL_ADD_EXECUTABLE( goalplanner main ${COMMON_SRCS} )
    
    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaGoalPlanner SHARED service ${COMMON_SRCS} )
    
    GENERATE_CONFIG_FILE( goalplanner.def goalplanner )

ENDIF ( BUILD_GOAL_PLANNER )
