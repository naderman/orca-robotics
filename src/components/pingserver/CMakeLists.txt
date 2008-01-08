SET_COMPONENT_NAMES ( PingServer )

REQUIRE_OPTION( EXE ${APP_NAME} ON )
REQUIRE_VAR( EXE ${APP_NAME} OS_LINUX "only Linux OS is supported" )
# Look for liboping -- library for 'ping'ing from C
INCLUDE ( ${HYDRO_HOME}/cmake/FindOPing.cmake )
REQUIRE_VAR( EXE ${APP_NAME} LIBOPING_FOUND "liboping not found" )
REQUIRE_VAR( EXE ${APP_NAME} HYDROPING_INSTALLED "libHydroPing not installed" )

IF ( BUILD )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE ( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS HydroPing )

    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${COMP_NAMESPACE} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
