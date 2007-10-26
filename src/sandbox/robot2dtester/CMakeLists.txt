IF ( NOT OS_WIN )

OPTION( BUILD_ROBOT2D_TESTER "Try to build a Tester for SegwayRmp component" OFF )

IF ( BUILD_ROBOT2D_TESTER )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_EXECUTABLE( robot2dtester ${SRCS} )

    GENERATE_CONFIG_FILE( robot2dtester.def )

ENDIF ( BUILD_ROBOT2D_TESTER )

ENDIF ( NOT OS_WIN )
