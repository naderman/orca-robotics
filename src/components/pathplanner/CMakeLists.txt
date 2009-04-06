ORCA_SET_COMPONENT_NAME( PathPlanner )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} GBX_OS_LINUX "only Linux OS is supported" )

set( dep_libs HydroOgMap HydroDll OrcaOgMap OrcaObjects OrcaIfaceImpl )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    file( GLOB srcs *.cpp )

    if( QT4_FOUND )
        include( ${HYDRO_CMAKE_DIR}/UseQt.cmake )
        add_definitions( -DQT4_FOUND )
    else( QT4_FOUND )
        list( REMOVE_ITEM srcs "${CMAKE_CURRENT_SOURCE_DIR}/skeletongraphicsI.cpp" )
        message( STATUS "     Qt4 not found: skeletongraphics will not be built" )
    endif( QT4_FOUND )
    
    # build stand-alone component  
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    if( ORCA_BUILD_SERVICES )
        ORCA_WRAP_MAINTHREAD_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
        target_link_libraries( ${SERVICE_NAME} ${dep_libs} )    
    endif( ORCA_BUILD_SERVICES )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
