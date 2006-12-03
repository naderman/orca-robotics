OPTION( BUILD_SIM_LOCALISER "Try to build SimLocaliser component" ON )

IF ( NOT WIN32 AND BUILD_SIM_LOCALISER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

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

#     INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src/libs/ )
#     LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} ${PROJECT_BINARY_DIR}/src/utils/orcaplayer )
#     ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS
        component mainloop localise2dI staticdriver
    )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( simlocaliser main ${COMMON_SRCS} )
    
    TARGET_LINK_LIBRARIES( simlocaliser ${STAGE_LIBS} )
    
    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaSimLocaliser SHARED service ${COMMON_SRCS} )
    TARGET_LINK_LIBRARIES( OrcaSimLocaliser ${STAGE_LIBS} )
    
    GENERATE_CONFIG_FILE( simlocaliser.def )

ENDIF ( NOT WIN32 AND BUILD_SIM_LOCALISER )
