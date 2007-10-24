IF ( OS_LINUX )

OPTION( BUILD_WALL "Try to build Wall utility" ON )

IF ( BUILD_WALL )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_EXECUTABLE( wall ${SRCS} )
    TARGET_LINK_LIBRARIES( wall OrcaWall HydroDll )

    GENERATE_CONFIG_FILE( wall.def )

ENDIF ( BUILD_WALL )
ENDIF ( OS_LINUX )
