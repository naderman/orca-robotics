# Glob all source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaSegwayRmpDriver SHARED ${SRCS} )
INSTALL_TARGETS( /lib OrcaSegwayRmpDriver )

IF ( BUILD_PEAKCAN )

  ADD_SUBDIRECTORY ( can )
  LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/can )
  TARGET_LINK_LIBRARIES( OrcaSegwayRmpDriver OrcaSegwayRmpCan pcan )

ENDIF ( BUILD_PEAKCAN ) 

IF ( FTD2XX_FOUND )

  ADD_SUBDIRECTORY( usb )
  LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/usb )
  TARGET_LINK_LIBRARIES( OrcaSegwayRmpDriver OrcaSegwayRmpUsb ftd2xx usb )

ENDIF ( FTD2XX_FOUND )

