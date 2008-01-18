SET_COMPONENT_NAMES ( PingServer )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD EXE ${APP_NAME} ON )
REQUIRE_VAR( BUILD EXE ${APP_NAME} OS_LINUX "only Linux OS is supported" )

SET( DEP_LIBS HydroPing )
REQUIRE_VAR( BUILD EXE ${APP_NAME} HYDROPING_INSTALLED "libHydroPing not installed" )

# Look for liboping just in case -- library for 'ping'ing from C
INCLUDE ( ${HYDRO_HOME}/cmake/FindOPing.cmake )
REQUIRE_VAR( BUILD EXE ${APP_NAME} LIBOPING_FOUND "liboping not found" )

IF ( BUILD )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE ( GLOB COMP_SRCS *.cpp )

    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${DEP_LIBS} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
