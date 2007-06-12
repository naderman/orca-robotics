OPTION( BUILD_SYSTEMTRACERMON "Try to build SystemTracerMon component" ON )

IF ( BUILD_SYSTEMTRACERMON )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    FILE ( GLOB SRCS *.cpp )

    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( systemtracermon ${SRCS} )

    GENERATE_CONFIG_FILE( systemtracermon.def )

ENDIF ( BUILD_SYSTEMTRACERMON )
