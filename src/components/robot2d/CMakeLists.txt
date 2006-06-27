OPTION( BUILD_ROBOT2D "Try to build Robot2d component" ON )

IF ( NOT WIN32 AND BUILD_ROBOT2D )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )
    
    # OPTIONAL_SUB_LIBRARY macro defined in component_rules.cmake
   
    OPTIONAL_SUB_LIBRARY(
        "USB driver for Segway RMP line of platforms"
        segwayrmpusb
        OrcaRobot2dSegwayRmpUsb
        SEGWAY_RMP_USB_LIBS
        FTD2XX_FOUND
        HAVE_USB_DRIVER
        ftd2xx usb )

#  OPTIONAL_SUB_LIBRARY( 
#    "CAN driver for Segway RMP line of platforms"
#    segwayrmpcan
#    OrcaRobot2dSegwayRmpCan
#    RM_CAN_LIBS
#    CANLIB_FOUND
#    HAVE_CAN_DRIVER
#    canlib )

# CMake problem: these should be in the subdir, but they have no effect there for some reason
INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )
    
    OPTIONAL_SUB_LIBRARY(
        "Player client driver for Robot2d"
        playerclient
        OrcaRobot2dPlayerClient
        PLAYERCLIENT_LIBS
        PLAYER_FOUND
        HAVE_PLAYERCLIENT_DRIVER
        ${PLAYER_LIBS} OrcaPlayer
    )

    IF ( NOT HAVE_USB_DRIVER AND NOT HAVE_PLAYERCLIENT_DRIVER )
        MESSAGE("    WARNING: only default 'fake' driver can be built")
    ENDIF ( NOT HAVE_USB_DRIVER AND NOT HAVE_PLAYERCLIENT_DRIVER )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS component nethandler hwhandler platform2dI powerI fakedriver )

    # Files for the component
    SET( COMP_SRCS main  ${COMMON_SRCS} )    
    
    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( robot2d ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( robot2d ${SEGWAY_RMP_USB_LIBS} ${PLAYERCLIENT_LIBS} )
    
    
    # Files for the service
    SET( SERV_SRCS  service  ${COMMON_SRCS} )
    
    # build an IceBox service
    ADD_LIBRARY( OrcaRobot2d SHARED ${SERV_SRCS} )
    TARGET_LINK_LIBRARIES( OrcaRobot2d ${SEGWAY_RMP_USB_LIBS} ${PLAYERCLIENT_LIBS} )
    
    INSTALL_TARGETS( /lib OrcaRobot2d )
    
    GENERATE_FROM_DEF( robot2d.def )

ENDIF ( NOT WIN32 AND BUILD_ROBOT2D )
