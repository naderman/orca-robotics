SET ( COMP_NAME simlocaliser )
SET ( SERVICE_NAME OrcaSimLocaliser )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${COMP_NAME} NOT_OS_WIN "Windows OS is not  supported" )

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

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl ${STAGE_LIBS} )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )
    
    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
