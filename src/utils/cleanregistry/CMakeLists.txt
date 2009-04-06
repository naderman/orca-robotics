ORCA_SET_COMPONENT_NAME( CleanRegistry )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )

set( dep_libs OrcaCompModel )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    file( GLOB srcs *.cpp )
    set( dep_libs ${dep_libs} IceGrid )  
    
    GBX_ADD_EXECUTABLE( ${APP_NAME} ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )
    
    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
