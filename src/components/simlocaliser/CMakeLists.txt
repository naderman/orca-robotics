IF ( NOT OS_WIN )

OPTION( BUILD_SIM_LOCALISER "Try to build SimLocaliser component" ON )

IF ( BUILD_SIM_LOCALISER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

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
    GLOBAL_ADD_EXECUTABLE( simlocaliser ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( simlocaliser ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaSimLocaliser SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaSimLocaliser ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )
    
    GENERATE_CONFIG_FILE( simlocaliser.def )

ENDIF ( BUILD_SIM_LOCALISER )
ENDIF ( NOT OS_WIN )
