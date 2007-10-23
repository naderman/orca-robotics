IF ( OS_LINUX )

# Look for Player
INCLUDE (${ORCA_CMAKE_DIR}/FindPlayer2.cmake)

IF ( PLAYER_FOUND)

    OPTION( BUILD_PLAYER2ORCA "Try to build Player-to-Orca example" ON )
    IF ( BUILD_PLAYER2ORCA )
    
        INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
        INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
        LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
        ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )
    
        GLOBAL_ADD_EXECUTABLE( player2orca main handler )
        TARGET_LINK_LIBRARIES( player2orca OrcaIfaceImpl ${PLAYER_LIBS} )
    
        GENERATE_CONFIG_FILE( player2orca.def )
    
    ENDIF ( BUILD_PLAYER2ORCA )

ELSE ( PLAYER_FOUND )
    MESSAGE ( STATUS "    Player not found: cannot build Player2Orca example" )
ENDIF ( PLAYER_FOUND )

ENDIF ( OS_LINUX )
