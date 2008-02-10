SET_COMPONENT_NAMES ( InsGps)

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} OFF )

SET( dep_libs OrcaIfaceImpl )
REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

SET( dep_libs ${dep_libs} HydroDll HydroInterfaces )
REQUIRE_VAR( build EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )
REQUIRE_VAR( build EXE ${APP_NAME} HYDROINTERFACES_INSTALLED "libHydroInterfaces not installed" )

IF ( build)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )

    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    # build IceBox service
    IF ( ORCA_BUILD_SERVICES AND NOT ORCA_OS_WIN )
        WRAP_MAINTHREAD_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${dep_libs} ) 
    ENDIF ( ORCA_BUILD_SERVICES AND NOT ORCA_OS_WIN )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
