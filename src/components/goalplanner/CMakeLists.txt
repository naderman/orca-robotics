OPTION( BUILD_GOAL_PLANNER "Try to build GoalPlanner" ON )

IF ( BUILD_GOAL_PLANNER )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS  )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( goalplanner ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( goalplanner ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaGoalPlanner SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaGoalPlanner ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )
    
    GENERATE_CONFIG_FILE( goalplanner.def )

ENDIF ( BUILD_GOAL_PLANNER )
