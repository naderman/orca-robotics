SET_COMPONENT_NAMES ( SystemTracerMon )

REQUIRE_OPTION( EXE ${APP_NAME} ON )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE ( GLOB SRCS *.cpp )

    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ${APP_NAME} ${SRCS} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
