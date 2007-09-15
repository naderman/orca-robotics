IF ( NOT OS_WIN )

OPTION( BUILD_ROBOT2D "Try to build Robot2d component" ON )

IF ( BUILD_ROBOT2D )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
    
    OPTIONAL_SUB_LIBRARY(
        "Player client driver for Robot2d"
        playerclient
        OrcaRobot2dPlayerClient
        PLAYERCLIENT_LIBS
        PLAYER_FOUND
        HAVE_PLAYERCLIENT_DRIVER
        ${PLAYER_LIBS} OrcaPlayer
    )
    INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
    ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )

    IF ( NOT HAVE_PLAYERCLIENT_DRIVER )
        MESSAGE( STATUS "    WARNING: only default 'fake' driver can be built")
    ENDIF ( NOT HAVE_PLAYERCLIENT_DRIVER )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl OrcaMisc ${PLAYERCLIENT_LIBS} )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( robot2d ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( robot2d ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaRobot2d SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaRobot2d ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )
    
    GENERATE_CONFIG_FILE( robot2d.def )

ENDIF ( BUILD_ROBOT2D )
ENDIF ( NOT OS_WIN )
