ORCA_SET_COMPONENT_NAME( RegistryList )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )

set( dep_libs OrcaCompModel )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    file( GLOB srcs *.cpp )
    
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
        
    if( NOT GBX_OS_WIN )
        set( dep_libs ${dep_libs} OrcaCompModel )
    else( NOT GBX_OS_WIN )
        set( dep_libs ${dep_libs} 
              optimized IceGrid debug IceGridd
              optimized Glacier2 debug Glacier2d )
    endif( NOT GBX_OS_WIN )
    target_link_libraries( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
