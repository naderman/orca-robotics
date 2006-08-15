OPTION( BUILD_GOAL_PLANNER "Try to build GoalPlanner" ON )

IF ( BUILD_GOAL_PLANNER )

    INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
    # Remove this line if you don't plan to use libOrcaIce
    INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( goalplanner ${SRCS} )
    GENERATE_CONFIG_FILE( goalplanner.def goalplanner )

ENDIF ( BUILD_GOAL_PLANNER )
