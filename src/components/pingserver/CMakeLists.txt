SET_COMPONENT_NAMES ( PingServer )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )
REQUIRE_VAR( build EXE ${APP_NAME} ORCA_OS_LINUX "only Linux OS is supported" )

SET( ext_libs HydroPing )
REQUIRE_INSTALLS( build EXE ${APP_NAME} ${ext_libs} )

# Look for liboping just in case -- library for 'ping'ing from C
INCLUDE ( ${HYDRO_HOME}/cmake/FindOPing.cmake )
REQUIRE_VAR( build EXE ${APP_NAME} LIBOPING_FOUND "liboping not found" )

IF ( build )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE ( GLOB srcs *.cpp )
    SET( dep_libs ${ext_libs} ${int_libs} ) 

    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
