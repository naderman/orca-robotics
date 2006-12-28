OPTION( BUILD_GOAL_PLANNER "Try to build GoalPlanner" ON )

IF ( BUILD_GOAL_PLANNER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( GoalPlannerSources )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( goalplanner main )
    TARGET_LINK_LIBRARIES( goalplanner GoalPlannerSources OrcaMisc )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaGoalPlanner SHARED service )
    TARGET_LINK_LIBRARIES( OrcaGoalPlanner GoalPlannerSources OrcaMisc )
    
    GENERATE_CONFIG_FILE( goalplanner.def )

ENDIF ( BUILD_GOAL_PLANNER )
