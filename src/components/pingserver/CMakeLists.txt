ORCA_SET_COMPONENT_NAME( PingServer )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} GBX_OS_LINUX "only Linux OS is supported" )

set( dep_libs HydroPing )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

# Look for liboping just in case -- a library for 'ping'ing from C
include( ${HYDRO_CMAKE_DIR}/FindOPing.cmake )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} LIBOPING_FOUND "liboping not found" )

if( build )
    
    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    file( GLOB srcs *.cpp )

    # build stand-alone component  
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
