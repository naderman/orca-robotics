# Glob all source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaSegwayRmpDriver SHARED ${SRCS} )
INSTALL_TARGETS( /lib OrcaSegwayRmpDriver )
#TARGET_LINK_LIBRARIES( OrcaSegwayRmpDriver )

OPTIONAL_SUB_LIBRARY(
  "USB driver for Segway RMP line of platforms"
  usb
  OrcaSegwayRmpUsb
  SEGWAY_RMP_USB_LIBS
  FTD2XX_FOUND
  HAVE_USB_DRIVER
  ftd2xx usb )

#ADD_SUBDIRECTORY( peakcan )
