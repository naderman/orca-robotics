ORCA_SET_COMPONENT_NAME( SegwayRmp )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )
find_package( Boost )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} Boost_FOUND "Boost is required" )

set( dep_libs HydroDll HydroInterfaces OrcaIfaceImpl OrcaObjects OrcaObjectUtil HydroRmpUtil OrcaEStopUtil )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

#    add_subdirectory( driverthread )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    file( GLOB srcs *.cpp )
#    set( dep_libs ${dep_libs} SegwayRmpDriverThread )
    set( dep_libs ${dep_libs} )

    # build stand-alone component
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )

    # build IceBox service
    if( ORCA_BUILD_SERVICES )
        ORCA_WRAP_MAINTHREAD_INTO_SERVICE( ${srcs} )
        target_link_libraries( ${SERVICE_NAME} ${dep_libs} )
    endif( ORCA_BUILD_SERVICES )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
