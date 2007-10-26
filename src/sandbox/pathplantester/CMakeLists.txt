OPTION( BUILD_PATHPLAN_TESTER "Try to build pathplan-tester" ON )

IF ( NOT OS_WIN AND BUILD_PATHPLAN_TESTER )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS
            algohandler
            component
            pathplanner2dconsumerI
    )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( pathplantester main ${COMMON_SRCS} )
    
    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaPathPlanTester SHARED service ${COMMON_SRCS} )

    GENERATE_CONFIG_FILE( pathplantester.def )

ENDIF ( NOT OS_WIN AND BUILD_PATHPLAN_TESTER )
