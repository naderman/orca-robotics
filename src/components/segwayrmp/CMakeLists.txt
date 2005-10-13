OPTION( BUILD_SEGWAYRMP "Try to build SegwayRMP component" OFF )

IF ( BUILD_SEGWAYRMP )

SUBDIRS( rmpusb )

INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )

# Not sure how to find "all .cpp files except main.cpp", have to do it by hand
SET( COMMON_SRCS rmpcomponent rmpFsm rmpcontext platform2d_i power_i )

# Files for the component
SET( COMP_SRCS main  COMMON_SRCS )

# new Seqway with USB bus
LINK_LIBRARIES( rmp ftd2xx usb )

LINK_DIRECTORIES( rmpusb )

# build a stand-alone component
GLOBAL_ADD_EXECUTABLE( segwayrmpusb ${COMP_SRCS} )


# Files for the service
SET( SERV_SRCS  rmpservice_i  COMMON_SRCS )

# build an IceBox service
ADD_LIBRARY( OrcaSegwayRmpUsb SHARED ${SERV_SRCS} )

INSTALL_TARGETS( /lib OrcaSegwayRmpUsb )

ENDIF ( BUILD_SEGWAYRMP )
