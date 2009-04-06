set( APP_NAME pureice )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )

if( build)

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIce.cmake )

    file( GLOB srcs *.cpp )
    
    GBX_ADD_EXECUTABLE( ${APP_NAME} ${srcs} )

endif( build )
