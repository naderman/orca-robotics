SET ( COMP_NAME player2orca )
SET ( COMP_NAMESPACE ${COMP_NAME} )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
REQUIRE_VAR( EXE ${COMP_NAME} OS_LINUX "only Linux OS is supported" )
INCLUDE (${HYDRO_HOME}/cmake/FindPlayer2.cmake)
REQUIRE_VAR( EXE ${COMP_NAME} PLAYER_FOUND "Player not found" )

IF ( BUILD)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_HOME}/cmake/UsePlayer.cmake )

#     INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
#     LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
#     ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )

#     SET( COMP_DEPS OrcaIfaceImpl ${PLAYER_LIBS} )
    SET( COMP_DEPS OrcaIfaceImpl )

    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${COMP_NAME} ${COMP_NAMESPACE} component mainthread )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
