ORCA_SET_COMPONENT_NAME( OrcaView2d )
set( APP_CTRLC_HANDLER 0 )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )

GBX_REQUIRE_VAR( build EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )

set( dep_libs HydroDll OrcaQGui OrcaQGuiElementModelView OrcaObjects )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    include( ${HYDRO_CMAKE_DIR}/UseQt.cmake )

    file( GLOB srcs *.cpp )

    # build stand-alone component
    ORCA_WRAP_COMPONENT_INTO_APP( ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
