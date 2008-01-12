SET ( LIB_NAME OrcaPlayer )
GLOBAL_ADD_LICENSE( GPL )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )

INCLUDE (${HYDRO_CMAKE_DIR}/FindPlayer2.cmake)
REQUIRE_VAR( LIB ${LIB_NAME} PLAYER_FOUND "Player not found" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UsePlayer.cmake )
    
    GLOBAL_ADD_LIBRARY( OrcaPlayer SHARED convertutils.cpp )
    INSTALL_FILES( /include/orcaplayer .*\\.h$ )

ENDIF ( BUILD )
