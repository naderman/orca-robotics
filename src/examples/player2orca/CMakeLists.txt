SET_COMPONENT_NAMES ( Player2Orca )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )
REQUIRE_VAR( build EXE ${APP_NAME} ORCA_OS_LINUX "only Linux OS is supported" )
INCLUDE (${HYDRO_HOME}/cmake/FindPlayer2.cmake)
REQUIRE_VAR( build EXE ${APP_NAME} PLAYER_FOUND "Player not found" )

SET( dep_libs OrcaIfaceImpl )
REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

IF ( build)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_HOME}/cmake/UsePlayer.cmake )

    FILE( GLOB srcs *.cpp )

    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
