SET ( COMP_NAME laser2og )
SET ( SERVICE_NAME OrcaFaithLocaliser )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
REQUIRE_VAR( EXE ${COMP_NAME} OS_LINUX "only Linux OS is supported" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS OrcaOgFusion OrcaPathPlan )
    
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS}  )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
