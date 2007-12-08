SET ( COMP_NAME systemtracermon )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE ( GLOB SRCS *.cpp )

    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${SRCS} )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
