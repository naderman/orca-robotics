SET ( COMP_NAME brick )
SET ( SERVICE_NAME OrcaBrick )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )

IF ( BUILD)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )

    # libs on which our code depends would go here
#     SET( COMP_DEPS ??? )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} main )
#     TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )

    # build IceBox service
    IF ( BUILD_SERVICES AND NOT OS_WIN )
        GLOBAL_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${COMP_SRCS} service )
#         TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES AND NOT OS_WIN )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
