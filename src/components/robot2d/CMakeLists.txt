SET_COMPONENT_NAMES ( Robot2d )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build EXE ${APP_NAME} NOT_ORCA_OS_WIN "Windows OS is not  supported" )

SET( dep_libs OrcaIfaceImpl OrcaObjectUtil OrcaRobotDriverUtil )
REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

SET( dep_libs ${dep_libs} HydroDll HydroInterfaces OrcaObjects )
REQUIRE_VAR( build EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )
REQUIRE_VAR( build EXE ${APP_NAME} HYDROINTERFACES_INSTALLED "libHydroInterfaces not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )

    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    IF ( ORCA_BUILD_SERVICES )
        WRAP_COMPONENT_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${srcs} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${dep_libs} ) 
    ENDIF ( ORCA_BUILD_SERVICES )
    
    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
