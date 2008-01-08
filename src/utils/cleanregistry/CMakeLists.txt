SET_COMPONENT_NAMES ( CleanRegistry )

REQUIRE_OPTION( EXE ${APP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${APP_NAME} NOT_OS_WIN "Windows OS is not  supported" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )
    SET( COMP_DEPS OrcaCompModel  IceGrid )  
    
    GLOBAL_ADD_EXECUTABLE( ${APP_NAME} ${SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${COMP_DEPS} )
    
    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
