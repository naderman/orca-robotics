# Look for Player
INCLUDE (${ORCA_CMAKE_DIR}/FindPlayer2.cmake)

IF ( NOT OS_WIN AND PLAYER_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UsePlayer.cmake )
    
    GLOBAL_ADD_LIBRARY( OrcaPlayer SHARED convertutils.cpp )

    INSTALL_FILES( /include/orcaplayer .*\\.h$ )

ELSE  ( NOT OS_WIN AND PLAYER_FOUND )

    MESSAGE( STATUS "Not Planning to Build Library: libOrcaPlayer -- Player not found")

ENDIF ( NOT OS_WIN AND PLAYER_FOUND )
