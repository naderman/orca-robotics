OPTION( BUILD_SEGWAYRMP "Try to build SegwayRMP component" ON )

IF ( BUILD_SEGWAYRMP )

SUBDIRS( rmpusb )

INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaiceutil.cmake )

# new Seqway with USB bus
# Note: the newer Debian libusb library is missing 2 functions
#           use an older library which came with ftd2xx driver.
LINK_LIBRARIES( rmp ftd2xx usb )
#SET( CMAKE_EXE_LINKER_FLAGS rmpusb/libusb-0.1.so.4.3.2 )

LINK_DIRECTORIES( rmpusb )

#
# This gathers all the source files into the variable 'SRCS'
#
#FILE( GLOB SRCS *.cpp )

# Files for the component
SET( COMP_SRCS main  rmpcomponent platform2d_i power_i )

# build a stand-alone component
GLOBAL_ADD_EXECUTABLE( segwayrmpusb ${COMP_SRCS} )

# Files for the service
SET( SERV_SRCS  rmpservice_i rmpcomponent platform2d_i power_i )

# build an IceBox service
ADD_LIBRARY( OrcaSegwayRmpUsb SHARED ${SERV_SRCS} )

INSTALL_TARGETS( /lib OrcaSegwayRmpUsb )

ENDIF ( BUILD_SEGWAYRMP )
