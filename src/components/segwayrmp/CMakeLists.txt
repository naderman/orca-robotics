OPTION( BUILD_SEGWAYRMP "Try to build SegwayRMP component" ON )

IF ( BUILD_SEGWAYRMP )

  INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
  INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )
 
  # OPTIONAL_SUB_LIBRARY macro defined in component_rules.cmake
   
  # USB driver
  OPTIONAL_SUB_LIBRARY( 
    "USB Driver for SegwayRMP"
    rmpusb
    OrcaSegwayRmpUsb
    HAVE_FTD2XX
    -DHAVE_USB_DRIVER
    ftd2xx usb )

  # CAN driver
  OPTIONAL_SUB_LIBRARY( 
    "CAN Driver for SegwayRMP"
    rmpcan
    OrcaSegwayRmpCan
    HAVE_CANLIB
    -DHAVE_CAN_DRIVER
    canlib )

  # PlayerClient driver (CAN segway only)
#  OPTIONAL_SUB_LIBRARY( 
#    "Player-client Driver for SegwayRMP"
#    rmpplayer
#    OrcaSegwayRmpPlayer
#    HAVE_PLAYER_1_6
#    -DHAVE_PLAYERCLIENT_DRIVER
#    playerclient )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS rmpcomponent rmpmainloop platform2d_i power_i rmpfakedriver )
    
    # Files for the component
    SET( COMP_SRCS main  COMMON_SRCS )    
    
    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( segwayrmp ${COMP_SRCS} )
    
    
    # Files for the service
    SET( SERV_SRCS  rmpservice_i  COMMON_SRCS )
    
    # build an IceBox service
    ADD_LIBRARY( OrcaSegwayRmp SHARED ${SERV_SRCS} )
    
    INSTALL_TARGETS( /lib OrcaSegwayRmp )

ENDIF ( BUILD_SEGWAYRMP )
