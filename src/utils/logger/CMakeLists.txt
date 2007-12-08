SET ( COMP_NAME logger )
SET ( SERVICE_NAME OrcaLogger )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${COMP_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( EXE ${COMP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaLog HydroDll )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF( BUILD )
