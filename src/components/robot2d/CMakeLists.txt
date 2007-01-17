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

    IF ( NOT HAVE_PLAYERCLIENT_DRIVER )
        MESSAGE( STATUS "    WARNING: only default 'fake' driver can be built")
    ENDIF ( NOT HAVE_PLAYERCLIENT_DRIVER )

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( Robot2dSources )
    
    SET( DEP_LIBS
      Robot2dSources OrcaIfaceImpl OrcaMisc ${PLAYERCLIENT_LIBS} )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( robot2d main )
    TARGET_LINK_LIBRARIES( robot2d ${DEP_LIBS} )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaRobot2d SHARED service )
    TARGET_LINK_LIBRARIES( OrcaRobot2d ${DEP_LIBS} )
    
    GENERATE_CONFIG_FILE( robot2d.def )

ENDIF ( BUILD_ROBOT2D )
ENDIF ( NOT OS_WIN )
