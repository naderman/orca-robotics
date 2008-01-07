SET ( COMP_NAME reactivewalker )
SET ( SERVICE_NAME OrcaReactiveWalkerService )
SET ( COMP_NAMESPACE ${COMP_NAME} )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )

IF ( BUILD)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB COMP_SRCS *.cpp )

    # libs on which our code depends would go here
#     SET( COMP_DEPS ??? )

    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${COMP_NAME} ${COMP_NAMESPACE} ${COMP_SRCS} )
#     TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )

    # build IceBox service
    IF ( BUILD_SERVICES )
        WRAP_COMPONENT_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_SRCS} )
#         TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
