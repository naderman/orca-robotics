OPTION( BUILD_LASER2D "Try to build Laser2d component" ON )

IF ( BUILD_LASER2D )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/laser2d )

    SET( WE_CAN_BUILD_SICK_CARMEN_DRIVER 0 )
    SET( WE_CAN_BUILD_PLAYERCLIENT_DRIVER 0 )
    IF ( OS_LINUX )

      # Has no outside dependencies, so we can always build it
      SET( WE_CAN_BUILD_SICK_CARMEN_DRIVER 1 )

      IF ( PLAYER_FOUND )
        SET( WE_CAN_BUILD_PLAYERCLIENT_DRIVER 1 )
        # these should be in the subdir, but they have no effect there for some reason
        INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
        LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
        ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )
      ENDIF ( PLAYER_FOUND )

    ENDIF ( OS_LINUX )

    OPTIONAL_SUB_LIBRARY(
        "Player client driver for Laser2d"
        playerclient
        OrcaLaser2dPlayerClient
        PLAYERCLIENT_LIBS
        WE_CAN_BUILD_PLAYERCLIENT_DRIVER
        HAVE_PLAYERCLIENT_DRIVER
        ${PLAYER_LIBS} OrcaPlayer
    )

    OPTIONAL_SUB_LIBRARY(
        "SICK Carmen driver for Laser2d"
        sickcarmen
        OrcaLaser2dSickCarmen
        SICKCARMEN_LIBS
        WE_CAN_BUILD_SICK_CARMEN_DRIVER
        HAVE_CARMEN_DRIVER
    )

    ADD_SUBDIRECTORY ( sickutil )

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( Laser2dSources )

    SET( DEP_LIBS
      Laser2dSources
      ${PLAYERCLIENT_LIBS}
      ${SICKCARMEN_LIBS}
      OrcaIfaceImpl )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( laser2d main )
    TARGET_LINK_LIBRARIES( laser2d ${DEP_LIBS} )
    
    IF ( NOT OS_WIN )
        # build IceBox service
        GLOBAL_ADD_LIBRARY( OrcaLaser2d SHARED service )
        TARGET_LINK_LIBRARIES( OrcaLaser2d ${DEP_LIBS} )
    ENDIF ( NOT OS_WIN )

    GENERATE_CONFIG_FILE( laser2d.def )

ENDIF ( BUILD_LASER2D )
