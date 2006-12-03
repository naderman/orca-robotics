IF ( NOT WIN32 AND PLAYER_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UsePlayer.cmake )
    
    GLOBAL_ADD_LIBRARY( OrcaPlayer SHARED convertutils.cpp )

    INSTALL_FILES( /include/orcaplayer .*\\.h$ )

ELSE  ( NOT WIN32 AND PLAYER_FOUND )

    MESSAGE( STATUS "Not Planning to Build Library: libOrcaPlayer -- Player not found")

ENDIF ( NOT WIN32 AND PLAYER_FOUND )
