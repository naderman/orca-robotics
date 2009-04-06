ORCA_SET_COMPONENT_NAME( SystemTracerMon )
set( COMP_NAMESPACE sysmon )
set( APP_CTRLC_HANDLER 0 )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} OFF )

set( dep_libs OrcaObjects )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    file( GLOB srcs *.cpp )

    # build stand-alone component  
    ORCA_WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
