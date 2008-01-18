SET ( LIB_NAME OrcaRobotDriverUtil )
GLOBAL_ADD_LICENSE( LGPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD LIB ${LIB_NAME} ON )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    INCLUDE_DIRECTORIES( 
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/src/libs
    )
    
    FILE( GLOB SRCS *.cpp )
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    INSTALL_FILES( /include/orcarobotdriverutil .*\\.h$ )

ENDIF ( BUILD )
