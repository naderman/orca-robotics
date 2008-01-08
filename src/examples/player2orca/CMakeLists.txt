SET_COMPONENT_NAMES ( Player2Orca )

REQUIRE_OPTION( EXE ${APP_NAME} ON )
REQUIRE_VAR( EXE ${APP_NAME} OS_LINUX "only Linux OS is supported" )
INCLUDE (${HYDRO_HOME}/cmake/FindPlayer2.cmake)
REQUIRE_VAR( EXE ${APP_NAME} PLAYER_FOUND "Player not found" )

IF ( BUILD)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_HOME}/cmake/UsePlayer.cmake )

    FILE( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS OrcaIfaceImpl )

    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
