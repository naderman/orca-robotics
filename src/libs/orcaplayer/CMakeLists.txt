IF ( NOT OS_WIN )

# Look for Player
INCLUDE (${ORCA_CMAKE_DIR}/FindPlayer2.cmake)

IF ( PLAYER_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UsePlayer.cmake )
    
    GLOBAL_ADD_LIBRARY( OrcaPlayer SHARED convertutils.cpp )
    INSTALL_FILES( /include/orcaplayer .*\\.h$ )

ELSE ( PLAYER_FOUND )
    GLOBAL_NOT_ADD_LIBRARY( OrcaPlayer "Player not found" )
ENDIF ( PLAYER_FOUND )
ENDIF ( NOT OS_WIN )
