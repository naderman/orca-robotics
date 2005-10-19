OPTION( BUILD_SEGWAYRMP "Try to build SegwayRMP component" OFF )

IF ( BUILD_SEGWAYRMP )

# USB driver
IF ( HAVE_FTD2XX )
    MESSAGE("    Can build USB driver for SegwayRmp")
    SUBDIRS( rmpusb )
    LINK_LIBRARIES( RmpUsb ftd2xx usb )
    LINK_DIRECTORIES( rmpusb )
ELSE ( HAVE_FTD2XX )
    MESSAGE("    Cannot build USB driver for SegwayRmp")
ENDIF ( HAVE_FTD2XX )

# CAN driver
IF ( HAVE_CANLIB )
    MESSAGE("    Can build CAN driver for SegwayRmp")
    SUBDIRS( rmpcan )
    LINK_LIBRARIES( RmpCan canlib )
    LINK_DIRECTORIES( rmpcan )
ELSE ( HAVE_CANLIB )
    MESSAGE("    Cannot build CAN driver for SegwayRmp")
ENDIF ( HAVE_CANLIB )

# fake driver has no dependencies
    MESSAGE("    Can build FAKE driver for SegwayRmp")
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
