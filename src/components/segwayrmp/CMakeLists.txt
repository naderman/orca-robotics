OPTION( BUILD_SEGWAYRMP "Try to build SegwayRMP component" OFF )

IF ( BUILD_SEGWAYRMP )

# USB driver
IF ( HAVE_FTD2XX )
    MESSAGE("    USB driver for SegwayRmp - ok")
    SUBDIRS( rmpusb )
    LINK_LIBRARIES( RmpUsb ftd2xx usb )
    LINK_DIRECTORIES( ${PROJ_BINARY_DIR}/src/components/segwayrmp/rmpusb )
ELSE ( HAVE_FTD2XX )
    MESSAGE("    USB driver for SegwayRmp - cannot")
ENDIF ( HAVE_FTD2XX )

# CAN driver
IF ( HAVE_CANLIB )
    MESSAGE("    CAN driver for SegwayRmp - ok")
    SUBDIRS( rmpcan )
    LINK_LIBRARIES( RmpCan canlib )
    LINK_DIRECTORIES( ${PROJ_BINARY_DIR}/src/components/segwayrmp/rmpcan )
ELSE ( HAVE_CANLIB )
    MESSAGE("    CAN driver for SegwayRmp - cannot")
ENDIF ( HAVE_CANLIB )

# libplayer driver (CAN segway only)
IF ( HAVE_PLAYER_1_6 )
    MESSAGE("    Player-client driver for SegwayRmp - ok")
    #SUBDIRS( rmpplayer )
    #LINK_LIBRARIES( RmpPlayer playerclient )
    #LINK_DIRECTORIES( ${PROJ_BINARY_DIR}/src/components/segwayrmp/rmpplayer )
ELSE ( HAVE_PLAYER_1_6 )
    MESSAGE("    Player-client driver for SegwayRmp - cannot")
ENDIF ( HAVE_PLAYER_1_6 )

# fake driver has no dependencies
    MESSAGE("    Fake driver for SegwayRmp - ok")
    SUBDIRS( rmpfake )
    LINK_LIBRARIES( RmpFake )
    LINK_DIRECTORIES( rmpfake )

# we can always build the FAKE driver, so continue with building the component

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )
    
    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS rmpcomponent rmpmainloop platform2d_i power_i )
    
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
