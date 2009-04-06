ORCA_SET_COMPONENT_NAME( SnapshotLogger )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( NOT_OS_WIN NOT OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} NOT_OS_WIN "Windows OS is not  supported" )

set( dep_libs HydroDll OrcaLog OrcaIfaceImpl )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    file( GLOB COMP_SRCS *.cpp )
    
    # build stand-alone component  
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    if( BUILD_SERVICES )
        ORCA_WRAP_MAINTHREAD_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
        target_link_libraries( ${SERVICE_NAME} ${dep_libs} )    
    endif( BUILD_SERVICES )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
