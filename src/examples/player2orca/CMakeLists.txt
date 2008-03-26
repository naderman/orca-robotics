SET_COMPONENT_NAMES ( Player2Orca )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} GBX_OS_LINUX "only Linux OS is supported" )
INCLUDE( ${HYDRO_CMAKE_DIR}/FindPlayer2.cmake )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} PLAYER_FOUND "Player not found" )

SET( dep_libs OrcaIfaceImpl )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

IF ( build)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UsePlayer.cmake )

    FILE( GLOB srcs *.cpp )

    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
