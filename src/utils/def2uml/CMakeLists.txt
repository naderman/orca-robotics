ORCA_SET_COMPONENT_NAME( Def2Uml )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )

set( dep_libs OrcaQUml )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

    # needs defintions from orca/ocm.ice
    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    file( GLOB srcs *.cpp )

    GBX_ADD_EXECUTABLE( ${APP_NAME} ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )

endif( build )
