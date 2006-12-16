OPTION( BUILD_LASER2D "Try to build Laser2d component" ON )

IF ( BUILD_LASER2D )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/laser2d )

    SET( WE_CAN_BUILD_SICK_CARMEN_DRIVER 0 )
    SET( WE_CAN_BUILD_PLAYERCLIENT_DRIVER 0 )
    IF ( NOT WIN32 AND NOT QNXNTO )

      # Has no outside dependencies, so we can always build it
      SET( WE_CAN_BUILD_SICK_CARMEN_DRIVER 1 )

      IF ( PLAYER_FOUND )
        SET( WE_CAN_BUILD_PLAYERCLIENT_DRIVER 1 )
        # these should be in the subdir, but they have no effect there for some reason
        INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
        LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
        ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )
      ENDIF ( PLAYER_FOUND )

    ENDIF ( NOT WIN32 AND NOT QNXNTO )

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

    FILE( GLOB SRCS *.cpp )
    # Set LIB_SRCS to everything _except_ main.cpp
    SET( LIB_SRCS ${SRCS} )
    LIST( REMOVE_ITEM LIB_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" )
    # Set COMP_SRCS to everything _except_ service.cpp
    SET( COMP_SRCS ${SRCS} )
    LIST( REMOVE_ITEM COMP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/service.cpp" )

    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( laser2d ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( laser2d ${PLAYERCLIENT_LIBS} ${SICKCARMEN_LIBS} )

    IF ( NOT WIN32 )
      # build an IceBox service
      GLOBAL_ADD_LIBRARY( OrcaLaser2d SHARED ${LIB_SRCS} )

      # important to let linker know about this inter-library dependency
      # otherwise IceBox fails to load the service
      TARGET_LINK_LIBRARIES( OrcaLaser2d ${PLAYERCLIENT_LIBS} ${SICKCARMEN_LIBS} )
    ENDIF ( NOT WIN32 )

    GENERATE_CONFIG_FILE( laser2d.def )

ENDIF ( BUILD_LASER2D )
