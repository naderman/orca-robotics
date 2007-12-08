SET ( COMP_NAME laserfeatureextractor )
SET ( SERVICE_NAME OrcaLaserFeatureExtractor )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
REQUIRE_VAR( EXE ${COMP_NAME} OS_LINUX "only Linux OS is supported" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
