ORCA_SET_LIBRARY_NAME( OrcaPlayer )
GBX_ADD_LICENSE( GPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )

include( ${HYDRO_CMAKE_DIR}/FindPlayer2.cmake )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} PLAYER_FOUND "Player not found" )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    include( ${HYDRO_CMAKE_DIR}/UsePlayer.cmake )
    
    file( GLOB hdrs *.h )

    GBX_ADD_LIBRARY( OrcaPlayer DEFAULT convertutils.cpp )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

endif( build )
