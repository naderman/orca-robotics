SET ( lib_name OrcaPlayer )
GLOBAL_ADD_LICENSE( GPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )

INCLUDE (${HYDRO_CMAKE_DIR}/FindPlayer2.cmake)
REQUIRE_VAR( build LIB ${lib_name} PLAYER_FOUND "Player not found" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UsePlayer.cmake )
    
    GLOBAL_ADD_LIBRARY( OrcaPlayer SHARED convertutils.cpp )
    INSTALL_FILES( /include/orcaplayer .*\\.h$ )

ENDIF ( build )
