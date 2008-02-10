SET_COMPONENT_NAMES ( GoalPlanner )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )

SET( dep_libs OrcaOgMap )
REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

SET( dep_libs ${dep_libs} HydroNavUtil OrcaObjects )
REQUIRE_VAR( build EXE ${APP_NAME} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )
    
    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    IF ( ORCA_BUILD_SERVICES )
        WRAP_MAINTHREAD_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${dep_libs} )    
    ENDIF ( ORCA_BUILD_SERVICES )
    
    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
