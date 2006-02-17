# $Id$

OPTION( BUILD_SEGWAYRMP "Try to build SegwayRmp component" ON )

IF ( BUILD_SEGWAYRMP )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )
    
    # OPTIONAL_SUB_LIBRARY macro defined in component_rules.cmake
   
    # USB driver
    OPTIONAL_SUB_LIBRARY(
        "USB driver for SegwayRmp"
        usb
        OrcaSegwayRmpUsb
        RMP_USB_LIBS
        FTD2XX_FOUND
        HAVE_USB_DRIVER
        ftd2xx usb )

  # CAN driver
#  OPTIONAL_SUB_LIBRARY( 
#    "CAN driver for SegwayRmp"
#    can
#    OrcaSegwayRmpCan
#    RM_CAN_LIBS
#    CANLIB_FOUND
#    HAVE_CAN_DRIVER
#    canlib )

# these should be in the subdir, but they have no effect there for some reason
INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )

    # OPTIONAL_SUB_LIBRARY macro defined in component_rules.cmake
    
    OPTIONAL_SUB_LIBRARY(
        "Player-client Driver for RoboticPlatform"
        playerclient
        OrcaRobotPlayerClient
        PLAYERCLIENT_LIBS
        PLAYER_FOUND
        HAVE_PLAYERCLIENT_DRIVER
        playerclient OrcaPlayer
    )

    IF ( NOT HAVE_USB_DRIVER AND NOT HAVE_CANLIB )
        MESSAGE("    WARNING: only default 'fake' driver can be built")
    ENDIF ( NOT HAVE_USB_DRIVER AND NOT HAVE_CANLIB )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS rmpcomponent nethandler hwhandler platform2dI powerI fakedriver )

    # Files for the component
    SET( COMP_SRCS main  ${COMMON_SRCS} )    
    
    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( segwayrmp ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( segwayrmp ${RMP_USB_LIBS} ${PLAYERCLIENT_LIBS} )
    
    
    # Files for the service
    SET( SERV_SRCS  rmpservice  ${COMMON_SRCS} )
    
    # build an IceBox service
    ADD_LIBRARY( OrcaSegwayRmp SHARED ${SERV_SRCS} )
    TARGET_LINK_LIBRARIES( OrcaSegwayRmp ${RMP_USB_LIBS} ${RM_CAN_LIBS} )
    
    INSTALL_TARGETS( /lib OrcaSegwayRmp )
    
    GENERATE_FROM_DEF( segwayrmp.def )

ENDIF ( BUILD_SEGWAYRMP )
