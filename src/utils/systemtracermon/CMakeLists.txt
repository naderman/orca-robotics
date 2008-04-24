ORCA_SET_COMPONENT_NAME( SystemTracerMon )
SET( COMP_NAMESPACE sysmon )
SET( APP_CTRLC_HANDLER 0 )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )

SET( dep_libs OrcaObjects )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )

    # build stand-alone component  
    ORCA_WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF( build )
