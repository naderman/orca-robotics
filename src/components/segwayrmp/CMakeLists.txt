SET_COMPONENT_NAMES ( SegwayRmp )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET ( NOT_GBX_OS_WIN NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} NOT_GBX_OS_WIN "Windows OS is not  supported" )

SET( ext_libs HydroDll HydroInterfaces )
GBX_REQUIRE_INSTALLS( build EXE ${APP_NAME} ${ext_libs} )

SET( int_libs OrcaIfaceImpl OrcaObjects OrcaObjectUtil OrcaRobotDriverUtil )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${int_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${ext_libs} ${int_libs} ) 
    
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
