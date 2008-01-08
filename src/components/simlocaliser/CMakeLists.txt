SET_COMPONENT_NAMES ( SimLocaliser )

REQUIRE_OPTION( EXE ${APP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${APP_NAME} NOT_OS_WIN "Windows OS is not  supported" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    # Look for Player
    INCLUDE (${HYDRO_HOME}/cmake/FindPlayer2.cmake)

    # these should be in the subdir, but they have no effect there for some reason
    INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
    LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
    ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )

    OPTIONAL_SUB_LIBRARY(
        "Stage Driver for SimLocaliser"
        stage
        OrcaSimLocaliserStage
        STAGE_LIBS
        PLAYER_FOUND
        HAVE_STAGE_DRIVER
        ${PLAYER_LIBS} OrcaPlayer
    )

    FILE( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS OrcaIfaceImpl ${STAGE_LIBS} )
    
    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        WRAP_MAINTHREAD_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_NAME}  ${COMP_SRCS} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} )    
    ENDIF ( BUILD_SERVICES )
    
    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
