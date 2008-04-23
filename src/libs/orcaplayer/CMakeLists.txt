SET( lib_name OrcaPlayer )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )

SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} not_os_win "Windows OS is not  supported" )

INCLUDE( ${HYDRO_CMAKE_DIR}/FindPlayer2.cmake )
GBX_REQUIRE_VAR( build LIB ${lib_name} PLAYER_FOUND "Player not found" )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UsePlayer.cmake )
    
    FILE( GLOB hdrs *.h )

    GBX_ADD_LIBRARY( OrcaPlayer SHARED convertutils.cpp )

    GBX_ADD_HEADERS( orcaplayer ${hdrs} )

ENDIF( build )
