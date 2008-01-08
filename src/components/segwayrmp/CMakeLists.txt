SET_COMPONENT_NAMES ( SegwayRmp )

REQUIRE_OPTION( EXE ${APP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${APP_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )
REQUIRE_VAR( EXE ${APP_NAME} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS HydroDll HydroInterfaces OrcaIfaceImpl OrcaObjectUtil OrcaRobotDriverUtil )
    
    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${COMP_DEPS} )

    # build IceBox service
    IF ( BUILD_SERVICES )
        WRAP_COMPONENT_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_SRCS} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
