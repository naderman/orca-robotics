OPTION( BUILD_SIM_LOCALISER "Try to build SimLocaliser component" ON )

IF ( NOT WIN32 AND BUILD_SIM_LOCALISER )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

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

#     INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} ${PROJ_SOURCE_DIR}/src/utils/ )
#     LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} ${PROJ_BINARY_DIR}/src/utils/orcaplayer )
#     ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS
        component mainloop localise2dI staticdriver
    )

    # Files for the component
    SET( COMP_SRCS main  ${COMMON_SRCS} )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( simlocaliser ${COMP_SRCS} )
    
    TARGET_LINK_LIBRARIES( simlocaliser ${STAGE_LIBS} )

    # Files for the service
    SET( SERV_SRCS service ${COMMON_SRCS} )
    
    # build an IceBox service
    ADD_LIBRARY( OrcaSimLocaliser SHARED ${SERV_SRCS} )
    TARGET_LINK_LIBRARIES( OrcaSimLocaliser ${STAGE_LIBS} )
    
    INSTALL_TARGETS( /lib OrcaSimLocaliser )
    
    GENERATE_FROM_DEF( simlocaliser.def )

ENDIF ( NOT WIN32 AND BUILD_SIM_LOCALISER )
